#pragma once

#include <interfaces/parsers/base_response_parser.h>
#include <interfaces/types/modbus_types.h>

namespace Interface
{

class ModbusResponseParser final : public BaseResponseParser
{
    Q_OBJECT
public:
    explicit ModbusResponseParser(QObject *parent = nullptr);

    bool isValid(const QByteArray &response) override;
    void parse(const QByteArray &response) override;
};

} // namespace Interface
