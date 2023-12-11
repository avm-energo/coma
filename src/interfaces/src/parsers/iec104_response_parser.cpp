#include "interfaces/parsers/iec104_response_parser.h"

#include <QDebug>

namespace Interface
{

using namespace Iec104;

Iec104ResponseParser::Iec104ResponseParser(QObject *parent) : BaseResponseParser(parent), m_isAPCIParseError(false)
{
}

void Iec104ResponseParser::updateControlBlock(const SharedControlBlock &newControlBlock) noexcept
{
    m_ctrlBlock = newControlBlock;
}

void Iec104ResponseParser::apciParseErrorHandle(const Iec104::ApciError err) noexcept
{
    m_isAPCIParseError = true;
    qWarning() << "Parsing APCI error: " << std_ext::to_underlying(err);
}

bool Iec104ResponseParser::isCompleteResponse()
{
    if (m_responseBuffer.size() < apciSize)
        return false;

    auto parseProduct = APCI::fromByteArray(m_responseBuffer.left(apciSize));
    if (parseProduct.has_value())
    {
        m_currentAPCI = *parseProduct;
        if (m_responseBuffer.size() == m_currentAPCI.m_asduSize + apciSize)
            return true;
        else
            return false;
    }
    else
    {
        apciParseErrorHandle(parseProduct.error());
        return true; // Because we don't want to wait an another data
    }
}

Error::Msg Iec104ResponseParser::validate()
{
    // Parse APCI check
    if (m_isAPCIParseError)
    {
        m_isAPCIParseError = false;
        return Error::WrongFormatError;
    }
    // Size check
    if (m_currentAPCI.m_ctrlBlock.m_format == FrameFormat::Information)
    {
        if (m_responseBuffer.size() != m_currentAPCI.m_asduSize + apciSize)
            return Error::SizeError;
    }
    return Error::NoError;
}

void Iec104ResponseParser::parse()
{
    ++m_ctrlBlock->m_received;
    switch (m_currentAPCI.m_ctrlBlock.m_format)
    {
    case FrameFormat::Information:
        parseInfoFormat();
        break;
    case FrameFormat::Supervisory:
        parseSupervisoryFormat();
        break;
    case FrameFormat::Unnumbered:
        parseUnnumberedFormat();
        break;
    }
    clearResponseBuffer();
}

void Iec104ResponseParser::parseInfoFormat() noexcept
{
    ;
}

void Iec104ResponseParser::parseSupervisoryFormat() noexcept
{
    /// Need action?
    ;
}

void Iec104ResponseParser::parseUnnumberedFormat() noexcept
{
    /// TODO: Send data to request parser?
    emit unnumberedFormatReceived(m_currentAPCI.m_ctrlBlock.m_func, m_currentAPCI.m_ctrlBlock.m_arg);
}

} // namespace Interface
