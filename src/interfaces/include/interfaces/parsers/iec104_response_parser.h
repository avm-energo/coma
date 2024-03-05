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

    /// \brief Используется для разбиения буффера входных данных на
    /// массив байт каждого ответа от устройства.
    void splitBuffer() noexcept;
    /// \brief Функция для обработки ошибок парсинга APCI входящих ответов.
    void apciParseErrorHandle(const Iec104::ApciError err) noexcept;

    /// \brief Парсинг данных в I-формате.
    void parseInfoFormat(const QByteArray &response) noexcept;
    /// \brief Парсинг данных в S-формате.
    void parseSupervisoryFormat() noexcept;
    /// \brief Парсинг данных в U-формате.
    void parseUnnumberedFormat() noexcept;

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
    /// \brief Сигнал для информирования исполнителя запросов
    /// о необходимости проверки контрольного блока APCI.
    void needToCheckControlBlock();
    /// \brief Сигнал для информирования исполнителя запросов
    /// о том, что запрошенные данные получены.
    void requestedDataReceived();
};

} // namespace Interface
