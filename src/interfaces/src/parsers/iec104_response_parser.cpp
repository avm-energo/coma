#include "interfaces/parsers/iec104_response_parser.h"

#include <QDebug>

namespace Interface
{

using namespace Iec104;

Iec104ResponseParser::Iec104ResponseParser(QObject *parent)
    : BaseResponseParser(parent), m_currentCommand(Command::None), m_unpacker(this)
{
    connect(&m_unpacker, &ASDUUnpacker::unpacked,   //
        this, &BaseResponseParser::responseParsed); //
    m_responses.reserve(1024);
}

void Iec104ResponseParser::updateControlBlock(const SharedControlBlock &newControlBlock) noexcept
{
    m_ctrlBlock = newControlBlock;
}

void Iec104ResponseParser::apciParseErrorHandle(const Iec104::ApciError err) noexcept
{
    /// TODO: So naive realization
    qWarning() << "Parsing APCI error: " << std_ext::to_underlying(err);
}

bool Iec104ResponseParser::isCompleteResponse()
{
    if (m_responseBuffer.size() >= apciSize)
        if (m_responseBuffer.size() >= (std::uint8_t(m_responseBuffer[1]) + 2))
            return true;
    return false;
}

Error::Msg Iec104ResponseParser::validate()
{
    return Error::NoError;
}

Error::Msg Iec104ResponseParser::validate(const QByteArray &response) noexcept
{
    auto parseProduct = APCI::fromByteArray(response.left(apciSize));
    // Parse APCI check
    if (parseProduct.has_value())
    {
        m_currentAPCI = *parseProduct;
        // Size check
        if (m_currentAPCI.m_ctrlBlock.m_format == FrameFormat::Information)
        {
            if (response.size() != m_currentAPCI.m_asduSize + apciSize)
                return Error::SizeError;
        }
        return validate(); // returns no error
    }
    else
    {
        apciParseErrorHandle(parseProduct.error());
        return Error::WrongFormatError;
    }
}

void Iec104ResponseParser::verify(const Iec104::ASDU &asdu) noexcept
{
    /// TODO: shitty code
    static bool confirm = false, terminate = false;
    switch (m_currentCommand)
    {
    case Command::RequestGroup:
        if (asdu.m_cause == CauseOfTransmission::ActivationConfirm)
            confirm = true;
        else if (asdu.m_cause == CauseOfTransmission::ActivationTermination)
            terminate = true;
        if (confirm && terminate)
        {
            confirm = false;
            terminate = false;
            m_currentCommand = Command::None;
            emit requestedDataReceived();
        }
        break;
    default:
        /// TODO: don't ignore other commands
        break;
    }
}

void Iec104ResponseParser::splitBuffer() noexcept
{
    while (isCompleteResponse())
    {
        auto responseSize = std::uint8_t(m_responseBuffer[1]) + 2;
        m_responses.emplace_back(m_responseBuffer.left(responseSize));
        m_responseBuffer.remove(0, responseSize);
    }
}

void Iec104ResponseParser::parse()
{
    splitBuffer();
    for (const auto &response : m_responses)
    {
        emit needToLog(QString("<- %1").arg(QString(response.toHex())), LogLevel::Info);
        auto validationResult = validate(response);
        if (validationResult == Error::Msg::NoError)
        {
            switch (m_currentAPCI.m_ctrlBlock.m_format)
            {
            case FrameFormat::Information:
                parseInfoFormat(response);
                break;
            case FrameFormat::Supervisory:
                parseSupervisoryFormat();
                break;
            case FrameFormat::Unnumbered:
                parseUnnumberedFormat();
                break;
            }
        }
        else
        {
            auto errStr = "Message validation fault, reason: " + QVariant::fromValue(validationResult).toString();
            emit needToLog(errStr, LogLevel::Error);
        }
    }
    m_responses.clear();
}

void Iec104ResponseParser::parseInfoFormat(const QByteArray &response) noexcept
{
    ++m_ctrlBlock->m_received;
    emit needToCheckControlBlock();
    auto asdu = ASDU::fromByteArray(response.mid(apciSize, m_currentAPCI.m_asduSize));
    m_unpacker.unpack(asdu);
    verify(asdu);
}

void Iec104ResponseParser::parseSupervisoryFormat() noexcept
{
    /// Need action?
}

void Iec104ResponseParser::parseUnnumberedFormat() noexcept
{
    /// TODO: Send data to request parser?
    emit unnumberedFormatReceived(m_currentAPCI.m_ctrlBlock.m_func, m_currentAPCI.m_ctrlBlock.m_arg);
}

void Iec104ResponseParser::receiveCurrentCommand(const Iec104::Command currCommand) noexcept
{
    m_currentCommand = currCommand;
}

} // namespace Interface
