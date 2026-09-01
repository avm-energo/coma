#include "interfaces/exec/iec104_query_executor.h"

#include <interfaces/parsers/iec104/iec104_request_parser.h>
#include <interfaces/parsers/iec104/iec104_response_parser.h>
#include <interfaces/types/serial_settings.h>

namespace Interface
{

using namespace Iec104;

Iec104QueryExecutor::Iec104QueryExecutor(RequestQueue &queue, IEC104Settings *settings, QObject *parent)
    : DefaultQueryExecutor(queue, settings, parent)
    , m_t1Timer(new QTimer(this))
    , m_t2Timer(new QTimer(this))
    , m_t3Timer(new QTimer(this))
{
    std::uint16_t w = settings->get<std::uint16_t>("w");
    std::uint16_t k = settings->get<std::uint16_t>("k");
    m_ctrlBlock = std::make_shared<ControlBlock>(FrameFormat::Information, k, w);

    m_t1Timer->setSingleShot(true);
    m_t1Timer->setInterval(settings->get<std::uint16_t>("t1") * 1000);
    m_t2Timer->setSingleShot(true);
    m_t2Timer->setInterval(settings->get<std::uint16_t>("t2") * 1000);
    m_t3Timer->setSingleShot(true);
    m_t3Timer->setInterval(settings->get<std::uint16_t>("t3") * 1000);

    // t1: партнёр слишком долго не подтверждает отправленное нами. Если это ожидание
    // STOPDT con при остановке - партнёр всё равно не ответил, форсируем остановку сами,
    // а не реконнект (реконнектиться в момент, когда нас просят отключиться, не нужно).
    connect(m_t1Timer, &QTimer::timeout, this,
        [this]()
        {
            if (m_isStopping)
                DefaultQueryExecutor::stop();
            else
                emit reconnectRequested();
        });
    // t2: мы слишком долго не подтверждаем партнёру принятое - шлём S-пакет сами.
    connect(m_t2Timer, &QTimer::timeout, this, &Iec104QueryExecutor::sendSupervisoryMessage);
    // t3: соединение простаивает - проверяем его живость тестовым кадром.
    connect(m_t3Timer, &QTimer::timeout, this, [this]() { testConnection(ControlArg::Activate); });
}

Iec104ResponseParser *Iec104QueryExecutor::getResponseParser() noexcept
{
    return reinterpret_cast<Iec104ResponseParser *>(m_responseParser);
}

Iec104RequestParser *Iec104QueryExecutor::getRequestParser() noexcept
{
    return reinterpret_cast<Iec104RequestParser *>(m_requestParser);
}

void Iec104QueryExecutor::closeConnection() noexcept
{
    auto stopMessage { getRequestParser()->createStopMessage() };
    writeToInterface(stopMessage, false);
}

void Iec104QueryExecutor::writeToInterface(const QByteArray &request, bool isCounted) noexcept
{
    if (!request.isEmpty())
    {
        emit sendDataToInterface(request);
        if (isCounted)
        {
            m_timeoutTimer.start();
            m_ctrlBlock->m_counters.addS();
            m_ctrlBlock->m_counters.resetNonAckPackets();
            m_t2Timer->stop(); // мы только что подтвердили партнёру приём - долг по w закрыт
            if (!m_t1Timer->isActive())
                m_t1Timer->start(); // ждём подтверждения на этот (и предыдущие неподтверждённые) I-пакет
        }
        writeToLog(request, Direction::ToDevice);
    }
}

void Iec104QueryExecutor::stop() noexcept
{
    // Реальный переход в Stopping откладываем: DefaultQueryExecutor::stop() здесь пока НЕ
    // вызываем - иначе цикл exec() тут же выйдет и заэмитит finished(), на который блокирующе
    // ждёт ConnectionContext::reset() перед закрытием сокета, не дождавшись STOPDT con.
    // Останавливаемся по-настоящему либо когда придёт STOPDT con (checkUnnumberedFormat),
    // либо когда истечёт t1, если партнёр так и не ответил.
    m_isStopping = true;
    closeConnection(); // STOPDT act
    if (!m_t1Timer->isActive())
        m_t1Timer->start();
}

void Iec104QueryExecutor::start()
{
    auto startMessage { getRequestParser()->createStartMessage() };
    m_ctrlBlock->m_counters.reset();
    writeToInterface(startMessage, false);
    // STARTDT_ACT не считается I-кадром (isCounted=false), поэтому writeToInterface не
    // взводит таймер тайм-аута сам. Взводим явно - иначе, если устройство не ответит
    // STARTDT_CON (например, ещё не готово после записи файла), исполнитель зависает в
    // Pending навсегда: реагировать на отсутствие ответа больше нечему.
    m_timeoutTimer.start();
    // t1 действует "при посылке или тестировании APDU" (ГОСТ 104, рис. 15) - ожидание
    // STARTDT_CON тоже считается t1, а не только I-пакеты и TESTFR.
    if (!m_t1Timer->isActive())
        m_t1Timer->start();
    setState(ExecutorState::Pending);
}

void Iec104QueryExecutor::receiveDataFromInterface(const QByteArray &response)
{
    m_timeoutTimer.start(); // restart timeout timer each time the next packet is received
    m_t3Timer->start();     // получение любого кадра (I, S или U) перезапускает t3 - ГОСТ 104, §5.2
    m_responseParser->accumulateToResponseBuffer(response);
    m_responseParser->parse();

    switch (getState())
    {
    case ExecutorState::Stopping:
    case ExecutorState::Pending:
        // Просто выходим из слота, если исполнителя
        // остановили, или запрошенные данные ещё не пришли
        break;
    default:
        run();
        break;
    }
}

void Iec104QueryExecutor::testConnection(ControlArg arg) noexcept
{
    auto testMessage { getRequestParser()->createTestMessage(arg) };
    // t1 действует "при посылке или тестировании APDU" (ГОСТ 104, стр. 43) - TESTFR-act
    // тоже должен ждать подтверждения, а не только I-пакеты.
    if (arg == ControlArg::Activate && !m_t1Timer->isActive())
        m_t1Timer->start();
    writeToInterface(testMessage, false);
}

void Iec104QueryExecutor::sendSupervisoryMessage() noexcept
{
    auto supervisoryMessage { getRequestParser()->createSupervisoryMessage() };
    writeToInterface(supervisoryMessage, false);
}

void Iec104QueryExecutor::checkUnnumberedFormat(const ControlFunc func, const ControlArg arg) noexcept
{
    switch (func)
    {
    case ControlFunc::StartDataTransfer:
        if (arg == ControlArg::Confirm) // send GI (0x00)
        {
            m_t1Timer->stop(); // STARTDT подтверждён - рис. 15
            CommandStruct command = { Commands::C_GI, 0, 0 };
            auto request = m_requestParser->parse(command);
            if (request.isEmpty())
                return;
            else
            {
                m_lastRequestedCommand.store(command.command);
                m_responseParser->setRequest(command);
                writeToInterface(request);
            }
        }
        break;
    case ControlFunc::StopDataTransfer:
        if (arg == ControlArg::Confirm && m_isStopping)
        {
            // STOPDT подтверждён вовремя - форсировать через t1 не нужно, останавливаемся
            // по-настоящему прямо сейчас. Раньше здесь стоял голый emit finished() - он
            // будил ожидающий ConnectionContext::reset(), но не переводил цикл exec() в
            // Stopping, и тот продолжал крутиться в своём потоке уже после закрытия сокета.
            m_t1Timer->stop();
            DefaultQueryExecutor::stop();
        }
        break;
    case ControlFunc::TestFrame:
        if (arg == ControlArg::Activate)
            testConnection(ControlArg::Confirm);
        else if (arg == ControlArg::Confirm) // партнёр подтвердил наш собственный TESTFR-act
            m_t1Timer->stop();
        break;
    }
}

void Iec104QueryExecutor::requestedDataReceived() noexcept
{
    m_timeoutTimer.stop();
    run(); // execute next query from queue
}

void Iec104QueryExecutor::sendFileReply(const FileReplyAction action, const quint8 fileNum, const quint8 section) noexcept
{
    auto reply = getRequestParser()->createFileReply(action, fileNum, section);
    writeToInterface(reply);
}

void Iec104QueryExecutor::sendFileWriteReply(
    const FileWriteReplyAction action, const quint8 fileNum, const quint8 section, const QByteArray &payload) noexcept
{
    // payload здесь - это данные ОДНОЙ секции (не всего файла), отрезанные в
    // Iec104ResponseParser::writeSectionSlice - секции размером более SECTIONSIZE устройство не примет.
    // Исключение - FileFinal: там payload это файл ЦЕЛИКОМ (см. createFileFinalMessage).
    switch (action)
    {
    case FileWriteReplyAction::SectionReady:
        writeToInterface(
            getRequestParser()->createSectionReadyMessage(fileNum, section, static_cast<quint32>(payload.size())));
        break;
    case FileWriteReplyAction::SendSegments:
        // Строим и сразу отправляем кадры по одному, не пачкой - иначе все они "запекут" в себе
        // один и тот же снимок N(S)/N(R), т.к. счётчики продвигаются только реальной отправкой.
        for (const auto &chunk : getRequestParser()->splitIntoSegments(payload))
            writeToInterface(getRequestParser()->buildSegmentFrame(fileNum, section, chunk));
        writeToInterface(getRequestParser()->buildLastSectionFrame(fileNum, section, payload));
        break;
    case FileWriteReplyAction::FileFinal:
        writeToInterface(getRequestParser()->createFileFinalMessage(fileNum, section, payload));
        break;
    }
}

void Iec104QueryExecutor::checkIPacketCounters(const RSCounter &counters) noexcept
{
    if (!m_ctrlBlock->m_counters.checkSentCounters(counters))
    {
        emit reconnectRequested();
        return;
    }

    m_ctrlBlock->m_counters.addR();
    if (!m_t2Timer->isActive())
        m_t2Timer->start(); // появился непогашенный долг перед партнёром - ждём t2, если сами не подтвердим раньше

    m_ctrlBlock->m_counters.setAck(counters);
    if (m_ctrlBlock->m_counters.unacknowledgedSentCount() == 0)
        m_t1Timer->stop(); // партнёр подтвердил всё, что мы ему отправляли

    if (m_ctrlBlock->m_counters.isWLimit())
    {
        sendSupervisoryMessage();
        m_ctrlBlock->m_counters.resetNonAckPackets();
        m_t2Timer->stop();
    }
}

void Iec104QueryExecutor::checkSPacketCounters(const RSCounter &counters) noexcept
{
    m_ctrlBlock->m_counters.setAck(counters);
    if (m_ctrlBlock->m_counters.unacknowledgedSentCount() == 0)
        m_t1Timer->stop();
}

} // namespace Interface
