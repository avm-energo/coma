#include "interfaces/parsers/modbus_response_parser.h"

namespace Interface
{

ModbusResponseParser::ModbusResponseParser(QObject *parent) : BaseResponseParser(parent)
{
}

bool ModbusResponseParser::isValid(const QByteArray &response)
{
    Q_UNUSED(response);
    return false;
}

void ModbusResponseParser::parse(const QByteArray &response)
{
    Q_UNUSED(response);
}

} // namespace Interface
