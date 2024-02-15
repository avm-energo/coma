#include "interfaces/parsers/modbus_response_parser.h"

#include <QDebug>
#include <gen/utils/crc16.h>
#include <interfaces/utils/modbus_convertations.h>

namespace Interface
{

constexpr quint8 errorModbusConst = 0x80;

ModbusResponseParser::ModbusResponseParser(QObject *parent)
    : BaseResponseParser(parent), m_deviceAddress(0), m_expectedRespSize(0)
{
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

bool ModbusResponseParser::isCompleteResponse()
{
    constexpr auto responseCmdPos = 1;
    constexpr auto minResponseSize = 5;
    constexpr auto reqFileSectionLengthPos = 7;

    if (m_responseBuffer.size() >= minResponseSize)
    {
        quint8 receivedCommand = m_responseBuffer[responseCmdPos];
        if (receivedCommand & errorModbusConst)
            return true;
        else if (m_request.command == Commands::C_ReqFile)
        {
            if (m_responseBuffer.size() >= 8)
            {
                // 10 байт - дополнительные данные команды Modbus
                // см. Расширение Modbus. Пользовательские функции
                quint8 sectionSize = m_responseBuffer[reqFileSectionLengthPos];
                m_expectedRespSize = sectionSize + 10;
            }
            else
                return false; // Не получили заголовок с длиной
        }
        if (m_responseBuffer.size() >= m_expectedRespSize)
            return true;
    }
    return false; // Пришло слишком мало данных
}

Error::Msg ModbusResponseParser::validate()
{
    if (validateCRC(m_responseBuffer))
    {
        if (validateDeviceAddress(m_responseBuffer))
            return Error::NoError;
        else
            return Error::WrongFormatError;
    }
    else
        return Error::CrcError;
}

void ModbusResponseParser::extractModbusData() noexcept
{
    m_responseBuffer.remove(0, 1); // Удаление адреса устройства
    m_responseBuffer.chop(2);      // Удаление CRC
}

void ModbusResponseParser::parse()
{
    extractModbusData();
    auto responseFunctionCode = static_cast<quint8>(m_responseBuffer.at(0));
    m_responseBuffer.remove(0, 1); // удаление кода функции из ответа
    if (responseFunctionCode & errorModbusConst)
    {
        processError(responseFunctionCode, m_responseBuffer);
        clearResponseBuffer();
        return;
    }
    const auto requestAddress = m_request.arg1.value<quint16>();
    if (!validateResponseSize(responseFunctionCode, m_responseBuffer))
    {
        clearResponseBuffer();
        return;
    }

    switch (responseFunctionCode)
    {
    case Modbus::FunctionCode::ReadCoils:
        processSinglePointSignals(m_responseBuffer, requestAddress);
        break;
    case Modbus::FunctionCode::ReadHoldingRegisters:
    case Modbus::FunctionCode::ReadInputRegister:
        if (m_request.command == Commands::C_ReqFloats || m_request.command == Commands::C_ReqStartup)
            processFloatSignals(m_responseBuffer, requestAddress);
        else
            processIntegerSignals(m_responseBuffer, requestAddress);
        break;
    // В ответе нет полезных данных, если не пришла ошибка, то команда прошла успешно
    case Modbus::FunctionCode::WriteMultipleRegisters:
        processOk();
        break;
    case Modbus::FunctionCode::ReadFileSection:
        processDataSection(m_responseBuffer);
        if (m_isLastSectionReceived)
        {
            quint32 count = m_request.arg2.toUInt();
            fileReceived(m_longDataBuffer, S2::FilesEnum(requestAddress), DataTypes::FileFormat(count));
            m_longDataBuffer.clear();
        }
        break;
    case Modbus::FunctionCode::WriteFileSection:
        if (m_isLastSectionSent)
        {
            processOk();
            m_isLastSectionSent = false;
        }
        break;
    default:
        qCritical() << "Parse: wrong function code, hex: " << m_responseBuffer.toHex();
        break;
    }
    clearResponseBuffer();
}

void ModbusResponseParser::processError(const quint8 functionCode, const QByteArray &response) noexcept
{
    auto responseExceptionCode(static_cast<Modbus::ExceptionCode>(response.at(0)));
    auto pureFunctionCode(static_cast<Modbus::FunctionCode>(functionCode - errorModbusConst));
    qWarning() << pureFunctionCode << " for address " << m_request.arg1.value<quint16>()
               << " device error: " << responseExceptionCode;
    // BaseResponseParser::processError(static_cast<int>(responseExceptionCode));
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
    for (int pos = 0, i = 0; pos < response.size(); pos += step, ++i)
    {
        DataTypes::FloatStruct signal;
        signal.sigAdr = address + i;
        signal.sigVal = Modbus::unpackRegister<float>(response.mid(pos, step));
        signal.sigQuality = DataTypes::Quality::Good;
        emit responseParsed(signal);
    }
}

void ModbusResponseParser::processIntegerSignals(const QByteArray &response, const quint16 address) noexcept
{
    constexpr auto step = sizeof(quint32);
    for (int pos = 0, i = 0; pos < response.size(); pos += step, ++i)
    {
        DataTypes::BitStringStruct signal;
        signal.sigVal = Modbus::unpackRegister<quint32>(response.mid(pos, step));
        signal.sigAdr = address + i;
        signal.sigQuality = DataTypes::Quality::Good;
        emit responseParsed(signal);
    }
}

void ModbusResponseParser::processDataSection(const QByteArray &dataSection) noexcept
{
    // Проверяем, получили мы последнюю секцию, или нет
    m_isLastSectionReceived = dataSection[0];
    // Получаем размер секции
    constexpr auto sectionSizePos = 5;
    auto sectionSize = static_cast<quint8>(dataSection[sectionSizePos]);
    // Добавляем полученные данные в буфер
    auto data = dataSection.mid(sectionSizePos + 1, sectionSize);
    bool ok = true;
    if ((data.size() != sectionSize) || data.size() < 16) // 16 - размер S2BHeader
    {
        qCritical() << "Получен некорректный размер при разборке секции файла";
        ok = false; // Not ok
    }
    m_longDataBuffer.append(data);
    if (m_isFirstSectionReceived) // Если получили первую секцию
    {
        if (!m_isLastSectionReceived)
            emit readingLongData();
        if (ok)
        {
            // Вытаскиваем размер из заголовка
            auto sizeBytes = m_longDataBuffer.mid(4, sizeof(quint32));
            auto size = *reinterpret_cast<quint32 *>(sizeBytes.data());
            if (size > 0)
                processProgressRange(size);
        }
    }

    // Отсылаем текущий прогресс
    processProgressCount(m_longDataBuffer.size() - sizeof(S2::S2FileHeader));
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
