#include "interfaces/parsers/iec104_response_parser.h"

#include <QDebug>

namespace Interface
{

using namespace Iec104;

Iec104ResponseParser::Iec104ResponseParser(QObject *parent) : BaseResponseParser(parent)
{
}

void Iec104ResponseParser::updateControlBlock(const SharedControlBlock &newControlBlock) noexcept
{
    m_ctrlBlock = newControlBlock;
}

void Iec104ResponseParser::apciErrorHandle(const Iec104::ApciError err) const noexcept
{
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
        auto asduSize = m_currentAPCI.m_asduSize;
        if (m_responseBuffer.size() == asduSize + apciSize)
            return true;
        else
            return false;
    }
    else
    {
        apciErrorHandle(parseProduct.error());
        return true; // Because we don't want to wait an another data
    }
}

Error::Msg Iec104ResponseParser::validate()
{
    return Error::NoError;
}

void Iec104ResponseParser::parse()
{
    clearResponseBuffer();
}

} // namespace Interface
