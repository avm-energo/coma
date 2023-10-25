#pragma once

#include <interfaces/parsers/base_request_parser.h>
#include <interfaces/types/modbus_types.h>

namespace Interface
{

class ModbusRequestParser final : public BaseRequestParser
{
    Q_OBJECT
private:
    quint8 m_deviceAddress;

    QByteArray createPDU(const Modbus::Request &request);

public:
    explicit ModbusRequestParser(QObject *parent = nullptr);

    QByteArray parse(const CommandStruct &cmd) override;
    QByteArray getNextContinueCommand() noexcept override;

    void setDeviceAddress(const quint8 deviceAddress) noexcept;

signals:
    void expectedResponseSize(const quint16 size);
};

} // namespace Interface
