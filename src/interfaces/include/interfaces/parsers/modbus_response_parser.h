#pragma once

#include <interfaces/parsers/base_response_parser.h>
#include <interfaces/types/modbus_types.h>

namespace Interface
{

class ModbusResponseParser final : public BaseResponseParser
{
    Q_OBJECT
private:
    QByteArray m_response;
    quint8 m_deviceAddress;
    quint16 m_expectedRespSize;

    /// \brief Функция для проверки размера полученного от устройства ответа.
    /// \details Ожидается, что полученный ответ будет соответствовать ожидаемому.
    /// \see expectedResponseSize.
    bool checkResponseLength(const QByteArray &response) noexcept;
    /// \brief Функция для проверки контрольной суммы (CRC16)
    /// из конца полученного от устройства ответа.
    bool validateCRC(const QByteArray &response) const noexcept;
    /// \brief Функция для проверки того, что ответ пришёл от
    /// устройства, с которым в данный момент установлена связь.
    bool validateDeviceAddress(const QByteArray &response) const noexcept;
    /// \brief Функция для проверки, что в ответ на запрос чтения
    /// данных был получен размер, указанный в начале ответа.
    bool validateResponseSize(const quint8 functionCode, QByteArray &response) const noexcept;

    /// \brief Для Modbus RTU удаляет протокольное обрамление
    /// в виде адреса устройства и CRC из полученного ответа.
    void removeModbusHeader() noexcept;

    void processError(const quint8 functionCode, const QByteArray &response) noexcept;
    void processSinglePointSignals(const QByteArray &response, const quint16 address) noexcept;
    void processFloatSignals(const QByteArray &response, const quint16 address) noexcept;
    void processIntegerSignals(const QByteArray &response, const quint16 address) noexcept;
    void processDataSection(const QByteArray &dataSection) noexcept;

public:
    explicit ModbusResponseParser(QObject *parent = nullptr);

    bool isValid(const QByteArray &response) override;
    void parse(const QByteArray &response) override;

    /// \brief Функция для задания адреса устройства,
    /// к которому будут формироваться запросы.
    /// \details Потенциально может быть перегружена для Modbus TCP.
    void setDeviceAddress(const quint8 deviceAddress) noexcept;

public slots:
    void expectedResponseSize(const quint16 size) noexcept;

signals:
    void needMoredata();
    void fullResponseReceived(const QByteArray &response);
};

} // namespace Interface
