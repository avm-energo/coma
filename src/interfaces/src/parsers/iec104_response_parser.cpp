#include "interfaces/parsers/iec104_response_parser.h"

namespace Interface
{

Iec104ResponseParser::Iec104ResponseParser(QObject *parent) : BaseResponseParser(parent)
{
}

void Iec104ResponseParser::updateControlBlock(const SharedControlBlock &newControlBlock) noexcept
{
    m_ctrlBlock = newControlBlock;
}

bool Iec104ResponseParser::isCompleteResponse()
{
    return true;
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
