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
    /// \brief Строит одиночную команду (C_SC_NA_1) по заданному адресу.
    /// \see Av-tuk.core: Com104Handler (M52_104.c) - адреса 801 (смена конфигурации), 802 (обновление ВПО).
    QByteArray createSingleCommand(const quint32 address, const bool value);
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
    /// \brief Строит сообщение "секция готова" в ответ на запрос устройством очередной секции.
    /// \param fileNum - номер файла, ТОТ ЖЕ, что был объявлен в createFileReadyMessage (не 0x01
    /// "по умолчанию" - устройство иначе ответит "несуществующее имя файла", см. ГОСТ 101 §7.2.6.30/32).
    /// \param section - номер секции (1-based, как в самом протоколе).
    /// \param sectionSize - размер именно ЭТОЙ секции (не всего файла) - см. SECTIONSIZE в прошивке.
    QByteArray createSectionReadyMessage(const quint8 fileNum, const quint8 section, const quint32 sectionSize) noexcept;
    /// \brief Разбивает данные СЕКЦИИ на куски по 230 байт - чистая функция, без обращения к счётчикам.
    /// \details Отдельно от построения кадров, чтобы вызывающий код (executor) мог строить и сразу
    /// отправлять кадры по одному - если построить все кадры заранее пачкой, все они "запекут" в себе
    /// один и тот же снимок N(S)/N(R), т.к. счётчики продвигаются только при реальной отправке.
    QList<QByteArray> splitIntoSegments(const QByteArray &payload) const noexcept;
    /// \brief Строит ОДИН кадр сегмента файла (F_SG_NA_1) из уже готового куска данных текущей секции.
    /// \warning Использует текущее состояние счётчиков m_ctrlBlock - вызывать непосредственно перед
    /// отправкой этого кадра, не заранее.
    QByteArray buildSegmentFrame(const quint8 fileNum, const quint8 section, const QByteArray &chunk) noexcept;
    /// \brief Строит завершающий кадр "конец секции" (F_LS_NA_1, qualifier=0x03) с контрольной
    /// суммой данных этой секции.
    /// \warning Как и buildSegmentFrame, использует текущее состояние счётчиков - вызывать непосредственно
    /// перед отправкой.
    QByteArray buildLastSectionFrame(const quint8 fileNum, const quint8 section, const QByteArray &sectionPayload) noexcept;
    /// \brief Строит завершающее сообщение файла (F_LS_NA_1, qualifier=0x01) в ответ на подтверждение
    /// последней секции устройством - секций больше нет, файл передан целиком.
    /// \param fullFilePayload - файл ЦЕЛИКОМ (не срез секции!) - контрольная сумма считается устройством
    /// нарастающим итогом по всем сегментам всех секций (KS_FileGet в File104.c, обнуляется только на
    /// F_FR_NA_1), поэтому чек-сумма только последней секции устройство отвергнет как NEG_FILE|ERRKS.
    QByteArray createFileFinalMessage(const quint8 fileNum, const quint8 section, const QByteArray &fullFilePayload) noexcept;

    QByteArray createStartMessage() const noexcept;
    QByteArray createStopMessage() const noexcept;
    QByteArray createTestMessage(Iec104::ControlArg arg) const noexcept;
    QByteArray createSupervisoryMessage() const noexcept;

signals:
    void currentCommand(const Iec104::Command cmd);
};

} // namespace Interface
