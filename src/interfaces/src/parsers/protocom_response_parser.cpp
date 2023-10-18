#include "interfaces/parsers/protocom_response_parser.h"

namespace Interface
{

ProtocomResponseParser::ProtocomResponseParser(QObject *parent) : BaseResponseParser(parent)
{
}

bool ProtocomResponseParser::isValid(const QByteArray &response)
{
    return false;
}

void ProtocomResponseParser::parse(const QByteArray &response)
{
    ;
}

} // namespace Interface
