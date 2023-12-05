#pragma once

#include <interfaces/parsers/base_request_parser.h>
#include <interfaces/types/iec104_types.h>

namespace Interface
{

class Iec104RequestParser final : public BaseRequestParser
{
    Q_OBJECT
private:
    SharedControlBlock m_ctrlBlock;
    quint16 m_baseStationAddress;

    QByteArray createGroupRequest(const quint32 groupNum);
    QByteArray createASDUPrefix(const Iec104::MessageDataType type, const quint32 address);

public:
    explicit Iec104RequestParser(QObject *parent = nullptr);

    void setBaseStationAddress(const quint16 bsAddress) noexcept;
    void updateControlBlock(const SharedControlBlock &newControlBlock) noexcept;

    QByteArray parse(const CommandStruct &cmd) override;
    QByteArray getNextContinueCommand() noexcept override;

    QByteArray createStartMessage() noexcept;
    QByteArray createStopMessage() noexcept;
};

} // namespace Interface
