#include "interfaces/parsers/modbus_response_parser.h"

#include <QDebug>
#include <execution>
#include <gen/utils/crc16.h>

namespace helper
{

template <typename T, std::size_t N = sizeof(T)> //
inline T unpackRegister(const QByteArray &ba)
{
    assert(N == ba.size());
    T value;
    auto dstBegin = reinterpret_cast<std::uint8_t *>(&value);
    std::copy(ba.cbegin(), ba.cend(), dstBegin);
    for (auto i = 0; i < N; i += 2)
        std::swap(dstBegin[i], dstBegin[i + 1]);
    return value;
}

} // namespace helper

namespace Interface
{

constexpr auto responseCmdPos = 1;
constexpr auto minResponseSize = 5;
constexpr auto reqFileSectionLengthPos = 7;
constexpr quint8 errorModbusConst = 0x80;

ModbusResponseParser::ModbusResponseParser(QObject *parent)
    : BaseResponseParser(parent), m_deviceAddress(0), m_expectedRespSize(0)
{
}

bool ModbusResponseParser::checkResponseLength(const QByteArray &response) noexcept
{
    if (response.size() >= minResponseSize)
    {
        quint8 receivedCommand = response[responseCmdPos];
        if (receivedCommand & errorModbusConst)
            return true;
        else if (m_request.command == Commands::C_ReqFile)
        {
            if (response.size() >= 8)
                m_expectedRespSize = response[reqFileSectionLengthPos] + 10; // дополнительные данные
            else
                return false; // Не получили заголовок с длиной
        }
        if (response.size() >= m_expectedRespSize)
            return true;
    }
    return false; // Пришло слишком мало данных
}

bool ModbusResponseParser::validateCRC(const QByteArray &response) const noexcept
{
    // Вытаскиваем CRC из ответа
    int rSize = response.size();
    quint16 receivedCRC =                                 //
        (static_cast<quint8>(response[rSize - 2]) << 8) | //
        (static_cast<quint8>(response[rSize - 1]));       //
    // Считаем CRC в ответе без учёта имеющейся CRC
    utils::CRC16 calculatedCRC(response.mid(0, rSize - 2));
    bool result = (calculatedCRC == receivedCRC);
    if (!result)
        qCritical() << Error::CrcError << metaObject()->className();
    return result;
}

bool ModbusResponseParser::validateDeviceAddress(const QByteArray &response) const noexcept
{
    auto responseAddress = static_cast<quint8>(response[0]);
    bool result = (responseAddress == m_deviceAddress);
    if (!result)
        qCritical() << "Пришёл ответ от устройства с другим адресом: " << responseAddress;
    return result;
}

bool ModbusResponseParser::validateResponseSize(const quint8 functionCode, QByteArray &response) const noexcept
{
    bool isValidSize = true;
    quint8 responseSize = 0;
    switch (functionCode)
    {
    // Проверяем размер только для ответов с чтением данных
    case Modbus::FunctionCode::ReadCoils:
    case Modbus::FunctionCode::ReadDiscreteInputs:
    case Modbus::FunctionCode::ReadHoldingRegisters:
    case Modbus::FunctionCode::ReadInputRegister:
        responseSize = static_cast<quint8>(response.at(0));
        response.remove(0, 1); // удаление размера из ответа
        isValidSize = (responseSize == response.size());
        break;
    default:
        break;
    }

    if (!isValidSize)
    {
        qCritical() << "Ошибка размера полученных данных, ожидали: " //
                    << responseSize << ", получили: " << response.size();
    }
    return isValidSize;
}

bool ModbusResponseParser::isValid(const QByteArray &response)
{
    m_response.append(response);
    if (checkResponseLength(m_response))
    {
        if (validateCRC(m_response) && validateDeviceAddress(m_response))
            return true;
        else
            m_response.clear();
    }
    else
        emit needMoredata(); // Парсеру надо больше данных от интерфейса
    return false;
}

void ModbusResponseParser::removeModbusHeader() noexcept
{
    m_response.remove(0, 1); // Удаление адреса устройства
    m_response.chop(2);      // Удаление CRC
}

void ModbusResponseParser::parse(const QByteArray &response)
{
    Q_UNUSED(response);
    removeModbusHeader();
    auto responseFunctionCode = static_cast<quint8>(m_response.at(0));
    m_response.remove(0, 1); // удаление кода функции из ответа
    if (responseFunctionCode & errorModbusConst)
    {
        processError(responseFunctionCode, m_response);
        m_response.clear();
        return;
    }
    const auto requestAddress = m_request.arg1.value<quint16>();
    if (!validateResponseSize(responseFunctionCode, m_response))
    {
        m_response.clear();
        return;
    }

    switch (responseFunctionCode)
    {
    case Modbus::FunctionCode::ReadCoils:
        processSinglePointSignals(m_response, requestAddress);
        break;
    case Modbus::FunctionCode::ReadHoldingRegisters:
    case Modbus::FunctionCode::ReadInputRegister:
        if (m_request.command == Commands::C_ReqFloats || m_request.command == Commands::C_ReqStartup)
            processFloatSignals(m_response, requestAddress);
        else
            processIntegerSignals(m_response, requestAddress);
        break;
    // В ответе нет полезных данных, если не пришла ошибка, то команда прошла успешно
    case Modbus::FunctionCode::WriteMultipleRegisters:
        processOk();
        break;
    case Modbus::FunctionCode::ReadFileSection:
        processDataSection(m_response);
        if (m_isLastSectionReceived)
        {
            quint32 count = m_request.arg2.toUInt();
            fileReceived(m_buffer, S2::FilesEnum(requestAddress), DataTypes::FileFormat(count));
        }
        break;
    case Modbus::FunctionCode::WriteFileSection:
        if (m_isLastSectionSended)
        {
            processOk();
            m_isLastSectionSended = false;
        }
        break;
    default:
        qCritical("We shouldn't be here, something went wrong");
        qCritical() << m_response.toHex();
        break;
    }
    m_response.clear();
}

void ModbusResponseParser::processError(const quint8 functionCode, const QByteArray &response) noexcept
{
    auto responseExceptionCode(static_cast<Modbus::ExceptionCode>(response.at(0)));
    auto pureFunctionCode(static_cast<Modbus::FunctionCode>(functionCode - errorModbusConst));
    qWarning() << pureFunctionCode << " error: " << responseExceptionCode;
    BaseResponseParser::processError(static_cast<int>(responseExceptionCode));
}

void ModbusResponseParser::processSinglePointSignals(const QByteArray &response, const quint16 address) noexcept
{
    DataTypes::SinglePointWithTimeStruct signal;
    for (int i = 0; i < response.size(); ++i)
    {
        std::bitset<8> byte(static_cast<quint8>(response[i]));
        for (int j = 0; j < 8; ++j)
        {
            signal.sigAdr = address + i * 8 + j;
            signal.sigVal = byte.test(j);
            signal.sigQuality = DataTypes::Quality::Good;
            emit responseParsed(signal);
        }
    }
}

void ModbusResponseParser::processFloatSignals(const QByteArray &response, const quint16 address) noexcept
{
    constexpr auto step = sizeof(float);
    for (auto i = 0; i < response.size(); i += step)
    {
        DataTypes::FloatStruct signal;
        signal.sigAdr = address + (i / step);
        signal.sigVal = helper::unpackRegister<float>(response.mid(i, step));
        signal.sigQuality = DataTypes::Quality::Good;
        emit responseParsed(signal);
    }
}

void ModbusResponseParser::processIntegerSignals(const QByteArray &response, const quint16 address) noexcept
{
    constexpr auto step = sizeof(quint32);
    for (auto i = 0; i < response.size(); i += step)
    {
        DataTypes::BitStringStruct signal;
        signal.sigVal = helper::unpackRegister<quint32>(response.mid(i, step));
        signal.sigAdr = address + i / step;
        signal.sigQuality = DataTypes::Quality::Good;
        emit responseParsed(signal);
    }
}

void ModbusResponseParser::processDataSection(const QByteArray &dataSection) noexcept
{
    // Проверяем, получили мы последнюю секцию, или нет
    m_isLastSectionReceived = dataSection[0];
    // Получаем размер секции
    auto sectionSize = static_cast<quint8>(dataSection[5]);
    // Добавляем полученные данные в буфер
    m_buffer.append(dataSection.mid(5, sectionSize));
    if (m_isFirstSectionReceived) // Если получили первую секцию
    {
        if (!m_isLastSectionReceived)
            emit readingLongData();
        /// TODO: По первой части полученного файла определять размер
    }
    processProgressCount(m_buffer.size()); // Отсылаем текущий прогресс
    // Восстанавливаем флаг, когда получаем последнюю секцию
    m_isFirstSectionReceived = m_isLastSectionReceived;
}

void ModbusResponseParser::setDeviceAddress(const quint8 deviceAddress) noexcept
{
    m_deviceAddress = deviceAddress;
}

void ModbusResponseParser::expectedResponseSize(const quint16 size) noexcept
{
    m_expectedRespSize = size;
}

} // namespace Interface
