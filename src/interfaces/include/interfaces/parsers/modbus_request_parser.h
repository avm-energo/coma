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
    quint16 m_fileNumber;
    quint16 m_fileSection;

    /// \brief Функция для создания PDU (Protocol Data Unit) согласно спецификации Modbus.
    /// \details Может бросать runtime_exception. Использовать с особой осторожностью.
    QByteArray createPDU(const Modbus::Request &request) const noexcept;
    /// \brief Функция для создания ADU (Application Data Unit) согласно спецификации Modbus.
    /// \see createPDU.
    QByteArray createADU(const Modbus::Request &request) const noexcept;

    /// \brief Возвращает запрос Modbus для чтения следующей секции файла.
    Modbus::Request getNextFileSectionRequest() noexcept;
    /// \brief Преобразовывает принятый файл в набор команд Modbus для записи файла.
    /// \details Принятый файл разделяется на секции, каждая из которых
    /// используется для формирования команды Modbus. Функцией возвращается
    /// команда на запись с первой секцией файла, остальные команды сразу преобразуются
    /// в очередь из объектов типа QByteArray для получения с помощью функции getNextDataSection.
    /// \see getNextDataSection.
    QByteArray prepareFileForWriting(const quint16 fileNum, const QByteArray &data) noexcept;

    /// \brief Функция для конвертации значений для записи
    /// устройства в формат запроса протокола Modbus.
    Modbus::Request convertUserValuesToRequest(const QVariantList &valuesList) const noexcept;

public:
    explicit ModbusRequestParser(QObject *parent = nullptr);

    /// \brief Функция парсинга запроса в бинарное представление в рамках протокола Modbus.
    QByteArray parse(const CommandStruct &cmd) override;
    /// \brief Функция для возврата следующей команды на
    /// запись большого массива данных или файла.
    QByteArray getNextContinueCommand() noexcept override;

    /// \brief Функция для задания адреса устройства,
    /// к которому будут формироваться запросы.
    void setDeviceAddress(const quint8 deviceAddress) noexcept;

signals:
    /// \brief Сигнал, который отправляет парсеру
    /// ответов ожидаемый размер ответа от устройства.
    void expectedResponseSize(const quint16 size) const; // clazy:exclude=const-signal-or-slot
};

} // namespace Interface
