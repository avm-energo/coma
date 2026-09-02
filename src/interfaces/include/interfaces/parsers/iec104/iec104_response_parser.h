#pragma once

#include <interfaces/parsers/base_response_parser.h>
#include <interfaces/types/iec104_types.h>

namespace Interface
{

/// \brief Класс парсера ответов от устройства по протоколу IEC-60870-5-104.
class Iec104ResponseParser final : public BaseResponseParser
{
    Q_OBJECT
private:
    SharedControlBlock m_ctrlBlock;
    std::vector<QByteArray> m_responses;
    Iec104::APCI m_currentAPCI;
    Iec104::Command m_currentCommand;
    Iec104::ASDUUnpacker m_unpacker;

    /// \brief ACTCON/ACTTERM получены для ТЕКУЩЕЙ команды (Command45/RequestGroup) - см. verify().
    /// \details Раньше это были static-локали внутри verify() - общее состояние "утекало" между
    /// РАЗНЫМИ командами: если у одной команды пришёл только ACTCON (например, устройство ушло в
    /// ResetSystem() после ACTCON на C_StartFirmwareUpgrade и не успело прислать ACTTERM), "confirm"
    /// оставался true навсегда и ошибочно "засчитывался" вместе с ACTTERM СЛЕДУЮЩЕЙ, никак не связанной
    /// команды - например, попап об успешном переходе на новое ПО показывался только после того, как
    /// пользователь отдельно записывал конфигурацию. Сбрасываются в receiveCurrentCommand() - на старте
    /// каждой новой команды, а не только по факту confirm+terminate.
    bool m_actConfirmed = false, m_actTerminated = false;

    /// \brief Номер текущей секции при ЗАПИСИ файла на устройство (1-based).
    /// \details Устройство принимает секции не больше SECTIONSIZE каждая (см. handleFileTransferAsdu) -
    /// в отличие от чтения файла, где номер секции присылает само устройство, при записи именно
    /// мы разбиваем payload на секции и должны сами помнить, на какой из них сейчас находимся.
    quint8 m_currentWriteSection = 1;

    /// \brief Кусок payload'а, соответствующий текущей секции записи (m_currentWriteSection).
    QByteArray writeSectionSlice(const QByteArray &payload) const noexcept;

    /// \brief Используется для разбиения буффера входных данных на
    /// массив байт каждого ответа от устройства.
    void splitBuffer() noexcept;

    /// \brief Парсинг данных в I-формате.
    void parseInfoFormat(const QByteArray &response) noexcept;
    /// \brief Парсинг данных в U-формате.
    void parseUnnumberedFormat() noexcept;
    /// \brief Обработка ASDU передачи файла (F_FR_NA_1..F_LS_NA_1).
    void handleFileTransferAsdu(const Iec104::ASDU &asdu) noexcept;
    /// \brief Проверка, относится ли тип ASDU к передаче файла.
    static bool isFileTransferType(const Iec104::MessageDataType type) noexcept;

    Error::Msg validate() override;
    Error::Msg validate(const QByteArray &response) noexcept;
    void verify(const Iec104::ASDU &asdu) noexcept;

public:
    /// \brief Конструктор по-умолчанию.
    explicit Iec104ResponseParser(QObject *parent = nullptr);
    /// \brief Сеттер для контрольного блока.
    void updateControlBlock(const SharedControlBlock &newControlBlock) noexcept;

    /// \brief Проверка размера ответа, полученного от устройства.
    bool isCompleteResponse() override;
    /// \brief Функция парсинга полученного от устройства ответа на запрос.
    void parse() override;

public slots:
    void receiveCurrentCommand(const Iec104::Command currCommand) noexcept;

signals:
    /// \brief Сигнал для информирования исполнителя
    /// запросов о пришедешей информации в U-формате.
    void unnumberedFormatReceived(const Iec104::ControlFunc func, const Iec104::ControlArg arg);
    /// \brief Пришел I-пакет.
    void infoTransferFormatReceived(const RSCounter counters);
    /// \brief Пришел S-пакет.
    void supervisoryFormatReceived(const RSCounter counters);
    /// \brief Сигнал для информирования исполнителя запросов
    /// о том, что запрошенные данные получены.
    void requestedDataReceived();
    /// \brief Сигнал для запроса у исполнителя отправки следующего
    /// шага реактивного протокола передачи файла устройству.
    void fileReplyNeeded(const Iec104::FileReplyAction action, const quint8 fileNum, const quint8 section);
    /// \brief Сигнал для запроса у исполнителя отправки следующего
    /// шага реактивного протокола записи файла в устройство.
    /// \param fileNum - номер файла, объявленный в исходном C_WriteFile (не жёстко заданная "1" -
    /// устройство иначе отвечает "несуществующее имя файла", ГОСТ 101 §7.2.6.30/32).
    /// \param section - номер секции, к которой относится это действие.
    /// \param payload - данные ИМЕННО этой секции (уже отрезанные от общего файла) для всех действий,
    /// КРОМЕ FileFinal - там передаётся файл целиком, т.к. контрольная сумма финального F_LS (qualifier=1)
    /// считается устройством нарастающим итогом по всему файлу, а не только по последней секции.
    void fileWriteReplyNeeded(const Iec104::FileWriteReplyAction action, const quint8 fileNum, const quint8 section,
        const QByteArray &payload);
};

} // namespace Interface
