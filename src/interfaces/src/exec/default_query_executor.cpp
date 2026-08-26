#include "interfaces/exec/default_query_executor.h"

#include <common/names.h>
#include <libavm-gen/settings.h>
#include <interfaces/parsers/base_request_parser.h>
#include <interfaces/parsers/base_response_parser.h>

namespace Interface
{

DefaultQueryExecutor::DefaultQueryExecutor(RequestQueue &queue, BaseSettings *settings, QObject *parent)
    : QObject(parent)
    , m_state(ExecutorState::Starting)
    , m_queue(std::ref(queue))
    , m_timeoutTimer(this)
    , m_requestParser(nullptr)
    , m_responseParser(nullptr)
{
    qRegisterMetaType<Interface::ExecutorState>("Interface::ExecutorState");
    setTimeout(settings->get(MemKeys::timeout));
    connect(settings, &BaseSettings::settingHasBeenChanged, this, &DefaultQueryExecutor::settingsChanged);
    connect(this, &DefaultQueryExecutor::stateChanged, this, &DefaultQueryExecutor::execByState);
}

void DefaultQueryExecutor::initLogger(const QString &protocolName) noexcept
{
    m_log.writeStart(protocolName + "Channel.log");
}

void DefaultQueryExecutor::setParsers(BaseRequestParser *reqParser, BaseResponseParser *respParser) noexcept
{
    if (reqParser != nullptr && respParser != nullptr)
    {
        m_requestParser = reqParser;
        m_responseParser = respParser;
        connect(m_responseParser, &BaseResponseParser::responseParsed,    //
            this, &DefaultQueryExecutor::responseSend);                   //
        connect(m_requestParser, &BaseRequestParser::totalWritingBytes,   //
            m_responseParser, &BaseResponseParser::processProgressRange); //
        connect(m_requestParser, &BaseRequestParser::progressBytes,       //
            m_responseParser, &BaseResponseParser::processProgressCount); //
        connect(m_requestParser, &BaseRequestParser::writingLastSection,  //
            m_responseParser, &BaseResponseParser::lastSectionSent);      //
        connect(m_requestParser, &BaseRequestParser::needToLog,           //
            this, &DefaultQueryExecutor::logFromParser);                  //
        connect(m_responseParser, &BaseResponseParser::needToLog,         //
            this, &DefaultQueryExecutor::logFromParser);                  //
        connect(m_responseParser, &BaseResponseParser::cancelRequest,     //
            this, &DefaultQueryExecutor::cancelQuery);                    //

        m_requestParser->basicProtocolSetup();                            // basic protocol setup
        connect(m_requestParser, &BaseRequestParser::writingLongData, this,
            [this]
            {
                setState(ExecutorState::WritingLongData);
                // m_timeoutTimer.setInterval(m_timeoutTimer.interval() * 5);
                m_queue.get().deactivate();
            });
        connect(m_responseParser, &BaseResponseParser::readingLongData, this,
            [this]
            {
                setState(ExecutorState::ReadingLongData);
                m_queue.get().deactivate();
            });
    }
}

ExecutorState DefaultQueryExecutor::getState() const noexcept
{
    return m_state.load();
}

void DefaultQueryExecutor::setState(const ExecutorState newState) noexcept
{
    if (getState() != ExecutorState::Stopping)
    {
        m_state.store(newState);
        emit stateChanged(newState);
    }
}

void DefaultQueryExecutor::wakeUp()
{
    // wakeUp() дёргают на каждый чих (любой полученный от устройства пакет,
    // не только новая команда в очереди - см. BaseInterface::poll()), поэтому
    // запускаем повторный разбор очереди, только если исполнитель реально
    // простаивает в ожидании следующей команды. Иначе можно было бы отправить
    // команду, пока предыдущая ещё не отработала.
    if (getState() != ExecutorState::RequestParsing)
        return;
    // Может вызываться из чужого потока (Qt::DirectConnection), поэтому сам
    // разбор очереди маршалим на поток исполнителя через AutoConnection -
    // на своём потоке отработает сразу же, с чужого - через очередь событий.
    QMetaObject::invokeMethod(this, &DefaultQueryExecutor::parseFromQueue, Qt::AutoConnection);
}

void DefaultQueryExecutor::parseFromQueue() noexcept
{
    // wakeUp() может поставить в очередь событий несколько вызовов этой
    // функции подряд (например, если executorWakeUp() прилетел пару раз,
    // пока предыдущий отложенный вызов ещё не обработан). Без этой проверки
    // второй вызов молча отправил бы следующую команду, не дождавшись ответа
    // на первую - протокол полудуплексный, это ломает разбор ответа.
    if (getState() != ExecutorState::RequestParsing)
        return;

    auto opt = m_queue.get().getFromQueue();
    if (opt.has_value())
    {
        const auto command(opt.value());
        auto request = m_requestParser->parse(command);
        if (m_requestParser->isExceptionalSituation())
            m_requestParser->exceptionalAction(command);
        else if (request.isEmpty())
            return;
        else
        {
            // m_requestParser->parse() для команд записи (C_WriteFile и т.п.)
            // может синхронно перевести состояние в WritingLongData (сигнал
            // writingLongData). В этом случае состояние трогать нельзя -
            // иначе long-data режим затрётся обратно на Pending и запись
            // оборвётся после первой секции.
            if (getState() == ExecutorState::RequestParsing)
                setState(ExecutorState::Pending);
            m_lastRequestedCommand.store(command.command);
            m_responseParser->setRequest(command);
            writeToInterface(request);
        }
    }
    // Если очереди пуста - просто выходим, не блокируя поток. Когда появится
    // новая команда, её приход разбудит нас через wakeUp().
}

void DefaultQueryExecutor::writeToInterface(const QByteArray &request, bool isCounted) noexcept
{
    Q_UNUSED(isCounted);
    emit sendDataToInterface(request);
    m_timeoutTimer.start();
    writeToLog(request, Direction::ToDevice);
}

void DefaultQueryExecutor::writeToLog(const QByteArray &ba, const Direction dir) noexcept
{
    QString msg = "DeviceQueryExecutor";
    switch (dir)
    {
    case Interface::FromDevice:
        msg += ": -> ";
        break;
    case Interface::ToDevice:
        msg += ": <- ";
        break;
    default:
        msg += ": ";
        break;
    }
    msg += ba.toHex();
    m_log.writeLog(Logger::Debug, msg);
}

void DefaultQueryExecutor::logFromParser(const QString &message, const Logger::MessageTypes level)
{
    m_log.writeLog(level, "DeviceQueryExecutor: " + message);
}

void DefaultQueryExecutor::settingsChanged(const QString &key, const QVariant &value)
{
    if (key == KeysMap.key(MemKeys::timeout))
        setTimeout(value.toInt());
}

void DefaultQueryExecutor::start()
{
    run();
}

void DefaultQueryExecutor::run() noexcept
{
    // Очередь активируем ДО смены состояния: setState() теперь синхронно
    // (через прямое соединение stateChanged->execByState) может сразу уйти
    // в parseFromQueue(). Если бы activate() стоял после setState(), очередь
    // на этот момент была бы ещё неактивна, и любой конкурентный addToQueue()
    // (из другого потока, пока мы внутри setState()) молча отбросил бы команду.
    m_queue.get().activate();
    setState(ExecutorState::RequestParsing);
}

void DefaultQueryExecutor::pause() noexcept
{
    setState(ExecutorState::Pending);
    m_queue.get().deactivate();
}

void DefaultQueryExecutor::stop() noexcept
{
    setState(ExecutorState::Stopping);
    m_queue.get().activate();
    m_queue.get().addToQueue({});
}

Commands DefaultQueryExecutor::getLastRequestedCommand() const noexcept
{
    return m_lastRequestedCommand.load();
}

void DefaultQueryExecutor::setTimeout(u32 timeout)
{
    QObject::disconnect(m_timeoutConnection);
    m_timeoutTimer.setSingleShot(true);
    m_timeoutTimer.setInterval(timeout);
    m_timeoutConnection = connect(&m_timeoutTimer, &QTimer::timeout, this,
        [this]
        {
            qDebug() << "Timeout, command: " << m_lastRequestedCommand.load();
            m_log.writeLog(Logger::Critical, "Timeout");
            cancelQuery();
            emit this->timeout();
            emit responseError(Error::Msg::Timeout);
        });
}

void DefaultQueryExecutor::receiveDataFromInterface(const QByteArray &response)
{
    m_timeoutTimer.start(); // restart timeout timer each time the next packet is received
    // Валидация при фрагментировании ответа от устройства
    m_responseParser->accumulateToResponseBuffer(response);
    if (m_responseParser->isCompleteResponse())
    {
        m_timeoutTimer.stop();
        writeToLog(m_responseParser->getResponseBuffer(), Direction::FromDevice);
    }
    else
        return;

    auto validationResult = m_responseParser->validate();
    if (validationResult == Error::NoError)
    {
        m_responseParser->parse();
        switch (getState())
        {
        case ExecutorState::ReadingLongData:
        {
            // Если не получили последнюю секцию, то посылаем команды на продолжение
            if (!m_responseParser->isLastSectionReceived())
            {
                auto request = m_requestParser->getNextContinueCommand();
                writeToInterface(request);
            }
            else
                run();
            break;
        }
        case ExecutorState::WritingLongData:
        {
            auto nextRequest = m_requestParser->getNextDataSection();
            // Если чанк не пустой, то ещё не отправили файл полностью
            if (!nextRequest.isEmpty())
                writeToInterface(nextRequest);
            // Если чанк пустой, то отправили файл полностью
            else
            {
                // m_timeoutTimer.setInterval(m_timeoutTimer.interval() / 5);
                run();
            }
            break;
        }
        case ExecutorState::Stopping:
            // Просто выходим из слота, если исполнителя остановили
            break;
        default:
            run();
            break;
        }
    }
    else
    {
        DataTypes::GeneralResponseStruct resp {
            DataTypes::GeneralResponseTypes::Error, //
            static_cast<quint64>(validationResult)  //
        };
        emit responseSend(resp);
        cancelQuery();
    }
}

void DefaultQueryExecutor::receiveProtocolDescription(const ProtocolDescription &desc) noexcept
{
    m_requestParser->updateProtocolSettings(desc);
}

void DefaultQueryExecutor::cancelQuery()
{
    m_log.writeLog(Logger::Warning, "Command canceled");
    m_responseParser->clearResponseBuffer();
    m_requestParser->clearLongDataSections();
    m_queue.get().activate();
    m_timeoutTimer.stop();
    setState(ExecutorState::RequestParsing);
    emit responseError(Error::Msg::Cancelled);
}

void DefaultQueryExecutor::reconnectEvent()
{
    m_log.writeLog(Logger::Warning, "Reconnect");
    cancelQuery();
    pause();
    m_queue.get().clear();
}

void DefaultQueryExecutor::execByState(const Interface::ExecutorState state)
{
    switch (state)
    {
    case ExecutorState::RequestParsing:
        parseFromQueue();

        break;
    case ExecutorState::Stopping:
        m_log.writeLog(Logger::Info, "DeviceQueryExecutor is finished\n");
        emit finished();

        break;
    default:
        // В остальных случаях ничего не делаем
        break;
    }
}

} // namespace Interface
