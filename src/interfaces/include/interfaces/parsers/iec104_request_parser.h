#pragma once

#include <interfaces/parsers/base_request_parser.h>
#include <interfaces/types/iec104_types.h>

namespace Interface
{

class Iec104RequestParser final : public BaseRequestParser
{
    Q_OBJECT
private:
    quint16 m_baseStationAddress;

    QByteArray createGroupRequest(const quint32 groupNum);

public:
    explicit Iec104RequestParser(QObject *parent = nullptr);

    void setBaseStationAddress(const quint16 bsAddress) noexcept;

    QByteArray parse(const CommandStruct &cmd) override;
    QByteArray getNextContinueCommand() noexcept override;
};

} // namespace Interface
