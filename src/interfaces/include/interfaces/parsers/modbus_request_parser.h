#pragma once

#include <interfaces/parsers/base_request_parser.h>
#include <interfaces/types/modbus_types.h>

namespace Interface
{

class ModbusRequestParser final : public BaseRequestParser
{
    Q_OBJECT
public:
    explicit ModbusRequestParser(QObject *parent = nullptr);

    QByteArray parse(const CommandStruct &cmd) override;
    QByteArray getNextContinueCommand() noexcept override;
};

} // namespace Interface
