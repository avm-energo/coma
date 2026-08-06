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
    /// \brief Разбивает данные файла на куски по 230 байт - чистая функция, без обращения к счётчикам.
    /// \details Отдельно от построения кадров, чтобы вызывающий код (executor) мог строить и сразу
    /// отправлять кадры по одному - если построить все кадры заранее пачкой, все они "запекут" в себе
    /// один и тот же снимок N(S)/N(R), т.к. счётчики продвигаются только при реальной отправке.
    QList<QByteArray> splitIntoSegments(const QByteArray &payload) const noexcept;
    /// \brief Строит ОДИН кадр сегмента файла (F_SG_NA_1) из уже готового куска данных.
    /// \warning Использует текущее состояние счётчиков m_ctrlBlock - вызывать непосредственно перед
    /// отправкой этого кадра, не заранее.
    QByteArray buildSegmentFrame(const QByteArray &chunk) noexcept;
    /// \brief Строит завершающий кадр "последняя секция, последний сегмент" (F_LS_NA_1) с контрольной
    /// суммой всего файла.
    /// \warning Как и buildSegmentFrame, использует текущее состояние счётчиков - вызывать непосредственно
    /// перед отправкой.
    QByteArray buildLastSectionFrame(const QByteArray &payload) noexcept;
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
