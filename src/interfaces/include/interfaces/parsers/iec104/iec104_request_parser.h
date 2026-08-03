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
    QByteArray buildFileFrame(
        const Iec104::MessageDataType type, const quint8 fileNum, const quint8 section, const QByteArray &tail);

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

    /// \brief Строит сообщение "файл готов" (начало передачи файла на запись).
    QByteArray createFileReadyMessage(const quint8 fileNum, const quint32 fileSize) noexcept;
    /// \brief Строит сообщение "секция готова" в ответ на запрос файла устройством.
    QByteArray createSectionReadyMessage(const quint32 fileSize) noexcept;
    /// \brief Строит поток сегментов файла (плюс завершающий "конец секции") в ответ на запрос устройства.
    QList<QByteArray> createSegmentBurst(const QByteArray &payload) noexcept;
    /// \brief Строит завершающее сообщение файла в ответ на подтверждение секции устройством.
    QByteArray createFileFinalMessage(const QByteArray &payload) noexcept;

    QByteArray createStartMessage() const noexcept;
    QByteArray createStopMessage() const noexcept;
    QByteArray createTestMessage(Iec104::ControlArg arg) const noexcept;
    QByteArray createSupervisoryMessage() const noexcept;

signals:
    void currentCommand(const Iec104::Command cmd);
};

} // namespace Interface
