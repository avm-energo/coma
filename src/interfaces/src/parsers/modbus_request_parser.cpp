#include "interfaces/parsers/modbus_request_parser.h"

namespace Interface
{

ModbusRequestParser::ModbusRequestParser(QObject *parent) : BaseRequestParser(parent)
{
}

QByteArray ModbusRequestParser::parse(const CommandStruct &cmd)
{
    Q_UNUSED(cmd);
    return {};
}

QByteArray ModbusRequestParser::getNextContinueCommand() noexcept
{
    return {};
};

} // namespace Interface
