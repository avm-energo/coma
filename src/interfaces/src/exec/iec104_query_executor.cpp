#include "interfaces/exec/iec104_query_executor.h"

#include <interfaces/parsers/iec104_request_parser.h>
#include <interfaces/parsers/iec104_response_parser.h>
#include <interfaces/types/serial_settings.h>

namespace Interface
{

using namespace Iec104;

Iec104QueryExecutor::Iec104QueryExecutor(RequestQueue &queue, IEC104Settings *settings, QObject *parent)
    : DefaultQueryExecutor(queue, settings, parent)
    , m_ctrlBlock(std::make_shared<ControlBlock>())
    , m_t2Timer(new QTimer(this))
    , m_t3Timer(new QTimer(this))
    , m_acknowledgeReceived(0)
{
    m_w = settings->get("w");
    m_t2Timer->setSingleShot(true);
    m_t2Timer->setInterval(settings->get<u16>("t2") * 1000);
    m_t3Timer->setSingleShot(true);
    m_t3Timer->setInterval(settings->get<u16>("t3") * 1000);
    connect(m_t2Timer, &QTimer::timeout, this, &Iec104QueryExecutor::sendSupervisoryMessage);
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

// void Iec104QueryExecutor::initConnection() noexcept { }

void Iec104QueryExecutor::closeConnection() noexcept
{
    auto stopMessage { getRequestParser()->createStopMessage() };
    writeToInterface(stopMessage, false);
}

void Iec104QueryExecutor::writeToInterface(const QByteArray &request, bool isCounted) noexcept
{
    emit sendDataToInterface(request);
    if (isCounted)
    {
        m_timeoutTimer.start();
        ++(m_ctrlBlock->m_sent);
        checkControlBlock();
    }
    writeToLog(request, Direction::ToDevice);
}

void Iec104QueryExecutor::exec()
{
    // initConnection();
    DefaultQueryExecutor::exec();
    // closeConnection();
}

void Iec104QueryExecutor::start()
{
    auto startMessage { getRequestParser()->createStartMessage() };
    writeToInterface(startMessage, false);
    setState(ExecutorState::Pending);
}

void Iec104QueryExecutor::receiveDataFromInterface(const QByteArray &response)
{
    m_timeoutTimer.start(); // restart timeout timer each time the next packet is received
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
    writeToInterface(testMessage, false);
}

void Iec104QueryExecutor::sendSupervisoryMessage() noexcept
{
    auto supervisoryMessage { getRequestParser()->createSupervisoryMessage() };
    m_acknowledgeReceived = m_ctrlBlock->m_received;
    writeToInterface(supervisoryMessage, false);
}

void Iec104QueryExecutor::checkControlBlock() noexcept
{
    auto triggerThresholdValue = m_acknowledgeReceived + m_w;
    if (m_ctrlBlock->m_received >= triggerThresholdValue || m_ctrlBlock->m_received == controlMax)
        sendSupervisoryMessage();
    if (m_ctrlBlock->m_received == controlMax)
        m_ctrlBlock->m_received = 0;
    if (m_ctrlBlock->m_sent == controlMax)
        m_ctrlBlock->m_sent = 0;
}

void Iec104QueryExecutor::checkUnnumberedFormat(const ControlFunc func, const ControlArg arg) noexcept
{
    switch (func)
    {
    case ControlFunc::StartDataTransfer:
        if (arg == ControlArg::Confirm) // send GI (0x00)
        {
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
        if (arg == ControlArg::Confirm)
            emit finished();
        break;
    case ControlFunc::TestFrame:
        if (arg == ControlArg::Activate)
            testConnection(ControlArg::Confirm);
        break;
    }
}

void Iec104QueryExecutor::requestedDataReceived() noexcept
{
    m_timeoutTimer.stop();
    run(); // execute next query from queue
}

} // namespace Interface
