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

    Protocol::Iec104Group getGroupByAddress(const quint32 addr) const noexcept;
    QByteArray createGroupRequest(const quint32 groupNum);
    QByteArray buildFileFrame(
        const Iec104::MessageDataType type, const quint8 fileNum, const quint8 section, const quint8 qualifier);

public:
    explicit Iec104RequestParser(QObject *parent = nullptr);
    void basicProtocolSetup() noexcept override;

    void setBaseStationAddress(const quint16 bsAddress) noexcept;
    void updateControlBlock(const SharedControlBlock &newControlBlock) noexcept;

    QByteArray parse(const CommandStruct &cmd) override;
    QByteArray getNextContinueCommand() noexcept override;
    void exceptionalAction(const CommandStruct &command) noexcept override;
    /// \brief Строит ответное сообщение для продолжения передачи файла по запросу от устройства.
    QByteArray createFileReply(const Iec104::FileReplyAction action, const quint8 fileNum, const quint8 section) noexcept;

    QByteArray createStartMessage() const noexcept;
    QByteArray createStopMessage() const noexcept;
    QByteArray createTestMessage(Iec104::ControlArg arg) const noexcept;
    QByteArray createSupervisoryMessage() const noexcept;

signals:
    void currentCommand(const Iec104::Command cmd);
};

} // namespace Interface
