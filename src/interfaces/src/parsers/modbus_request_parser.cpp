#include "interfaces/parsers/modbus_request_parser.h"

#include <gen/utils/crc16.h>
#include <stdexcept>

namespace helper
{

template <typename T, std::size_t N = sizeof(T)> //
inline QByteArray packRegister(T value)
{
    static_assert(N % 2 == 0, "The size of type T must be even");
    static_assert(N >= 2, "The size of type T must be greater than or equal to 2");
    QByteArray ba;
    auto srcBegin = reinterpret_cast<std::uint8_t *>(&value);
    auto srcEnd = srcBegin + N;
    for (auto it = srcBegin; it != srcEnd; it = it + 2)
    {
        ba.push_back(*(it + 1));
        ba.push_back(*it);
    }
    return ba;
}

} // namespace helper

namespace Interface
{

ModbusRequestParser::ModbusRequestParser(QObject *parent) : BaseRequestParser(parent), m_deviceAddress(0)
{
}

QByteArray ModbusRequestParser::parse(const CommandStruct &cmd)
{
    m_request.clear();
    Modbus::Request request {};
    switch (cmd.command)
    {
    // Reading coils
    case Commands::C_ReqAlarms:
    {
        request = Modbus::Request {
            Modbus::FunctionCode::ReadCoils,  //
            cmd.arg1.value<quint16>(),        // нехорошо, т.к. кладём туда quint32
            cmd.arg2.value<quint16>(), false, //
            {}                                //
        };
        break;
    }
    // Reading registers
    case Commands::C_ReqStartup:
    case Commands::C_ReqFloats:
    case Commands::C_ReqBitStrings:
    case Commands::C_ReqBSI:
    case Commands::C_ReqBSIExt:
    {
        const quint8 count = (cmd.arg2.value<quint16>() * 2); // startup registers are float (2 ints long)
        request = Modbus::Request {
            Modbus::FunctionCode::ReadInputRegister, //
            cmd.arg1.value<quint16>(),               // нехорошо, т.к. кладём туда quint32
            count, false, {}                         //
        };
        break;
    }
    // writing registers
    case Commands::C_ReqTime:
    {
        request = Modbus::Request {
            Modbus::FunctionCode::ReadHoldingRegisters, //
            Modbus::timeReg, 2, false, {}               //
        };
        break;
    }
    // request file
    case Commands::C_ReqFile:
    {
        m_fileNumber = cmd.arg1.value<quint16>();
        m_fileSection = 0;
        request = getNextFileSectionRequest();
        break;
    }
    // write file
    case Commands::C_WriteFile:
    {
        auto fileNum = cmd.arg1.value<quint16>();
        auto fileData = cmd.arg2.value<QByteArray>();
        return prepareFileForWriting(fileNum, fileData);
    }
    // writing registers
    case Commands::C_StartFirmwareUpgrade:
    {
        auto value = helper::packRegister(quint16(1));
        request = Modbus::Request {
            Modbus::FunctionCode::WriteMultipleRegisters, //
            Modbus::firmwareAddr, 1, false, value         //
        };
        break;
    }
    // writing registers
    case Commands::C_WriteTime:
    {
        QByteArray timeArray = helper::packRegister(cmd.arg1.value<quint32>());
        request = Modbus::Request {
            Modbus::FunctionCode::WriteMultipleRegisters, //
            Modbus::timeReg, 2, false, timeArray          //
        };
        break;
    }
    // writing registers
    case Commands::C_WriteUserValues:
    {
        if (cmd.arg1.canConvert<QVariantList>())
        {
            auto valuesList = cmd.arg1.value<QVariantList>();
            request = convertUserValuesToRequest(valuesList);
        }
        break;
    }
    // writing registers
    case Commands::C_WriteSingleCommand:
    {
        if (cmd.arg1.canConvert<DataTypes::SingleCommand>())
        {
            auto singleCmd = cmd.arg1.value<DataTypes::SingleCommand>();
            request = Modbus::Request {
                Modbus::FunctionCode::WriteMultipleRegisters, //
                singleCmd.addr, 1, false,                     //
                StdFunc::toByteArray(singleCmd.value)         //
            };
        }
        break;
    }
    // writing registers
    case Commands::C_WriteHardware:
    {
        if (cmd.arg1.canConvert<DataTypes::BlockStruct>())
        {
            auto value = cmd.arg1.value<DataTypes::BlockStruct>().data;
            const quint16 quantity = value.size() / 2;
            request = Modbus::Request {
                Modbus::FunctionCode::WriteMultipleRegisters,   //
                Modbus::hardwareVerAddr, quantity, false, value //
            };
        }
        break;
    }
    // writing registers
    case Commands::C_EnableWritingHardware:
    {
        auto value = helper::packRegister(cmd.arg1.value<quint16>());
        request = Modbus::Request {
            Modbus::FunctionCode::WriteMultipleRegisters, //
            Modbus::enableWriteHwAddr, 1, false, value    //
        };
        break;
    }
    // "WS" commands
    case Commands::C_ClearStartupError:
    case Commands::C_ClearStartupUnbounced:
    case Commands::C_ClearStartupValues:
    case Commands::C_SetStartupValues:
    case Commands::C_SetStartupPhaseA:
    case Commands::C_SetStartupPhaseB:
    case Commands::C_SetStartupPhaseC:
    case Commands::C_SetStartupUnbounced:
    case Commands::C_StartWorkingChannel:
    case Commands::C_SetTransOff:
    {
        auto search = s_wsCmdMap.find(cmd.command);
        if (search != s_wsCmdMap.cend())
        {
            auto value = helper::packRegister(cmd.arg1.value<quint16>());
            request = Modbus::Request {
                Modbus::FunctionCode::WriteMultipleRegisters,     //
                static_cast<quint16>(s_wsCmdMap.at(cmd.command)), //
                1, false, value                                   //
            };
        }
        break;
    }
    default:
        qCritical() << "Undefined command: " << cmd.command;
    }
    m_request = createADU(request);
    return m_request;
}

QByteArray ModbusRequestParser::getNextContinueCommand() noexcept
{
    return createADU(getNextFileSectionRequest());
};

void ModbusRequestParser::setDeviceAddress(const quint8 deviceAddress) noexcept
{
    m_deviceAddress = deviceAddress;
}

QByteArray ModbusRequestParser::createPDU(const Modbus::Request &request) const noexcept
{
    quint16 bytesToReceive = 0, temp = 0;
    QByteArray pdu;
    pdu.append(request.code);
    pdu.append(StdFunc::toByteArray(qToBigEndian(request.address)));
    pdu.append(StdFunc::toByteArray(qToBigEndian(request.quantity)));

    switch (request.code)
    {
    case Modbus::FunctionCode::ReadCoils:
        temp = (request.quantity / 8) + ((request.quantity % 8) != 0);
        bytesToReceive = temp + 5; // address (1), function code (1), bytes count (1), crc (2)
        break;
    case Modbus::FunctionCode::ReadDiscreteInputs:
        bytesToReceive = request.quantity + 5; // address (1), function code (1), bytes count (1), crc (2)
        break;
    case Modbus::FunctionCode::ReadHoldingRegisters:
    case Modbus::FunctionCode::ReadInputRegister:
        bytesToReceive = request.quantity * 2 + 5; // address (1), function code (1), bytes count (1), crc (2)
        break;
    case Modbus::FunctionCode::WriteMultipleRegisters:
        bytesToReceive = 8; // address (1), function code (1), address (2), quantity (2), crc (2)
        if (request.quantity * 2 != request.data.size())
        {
            qCritical() << "Incorrect quantity";
            return QByteArray {};
        }
        if (request.data.size() == 0)
        {
            qCritical() << "Incorrect command data";
            return QByteArray {};
        }
        pdu.append(quint8(request.quantity * 2));
        pdu.append(request.data);
        break;
    // Во время чтения файлов response parser сам определяет размер ответа
    case Modbus::FunctionCode::ReadFileSection:
        pdu.append(request.data);
        break;
    case Modbus::FunctionCode::WriteFileSection:
        // Response size: address (1), function code (1), last section (1),
        // numFile (2), numSection (2), secLength (1), crc (2)
        bytesToReceive = 10;
        pdu.insert(1, char(request.isLastSection));
        pdu.append(quint8(request.data.size()));
        pdu.append(request.data);
        break;
    default:
        break;
    }

    if (bytesToReceive > 0)
        emit expectedResponseSize(bytesToReceive);
    return pdu;
}

QByteArray ModbusRequestParser::createADU(const Modbus::Request &request) const noexcept
{
    QByteArray adu = createPDU(request);
    if (!adu.isEmpty())
    {
        adu.prepend(m_deviceAddress);
        utils::CRC16 crc(adu);
        adu.append(crc.toByteArray());
    }
    return adu;
}

Modbus::Request ModbusRequestParser::getNextFileSectionRequest() noexcept
{
    return Modbus::Request {
        Modbus::FunctionCode::ReadFileSection,          //
        m_fileNumber,                                   // номер файла
        m_fileSection++, false,                         // номер секции
        StdFunc::toByteArray(Modbus::fileSectionLength) //
    };
}

QByteArray ModbusRequestParser::prepareFileForWriting(const quint16 fileNum, const QByteArray &data) noexcept
{
    std::deque<Modbus::Request> requestBuffer;
    quint16 section = 0;
    m_progressCount = 0;
    m_longDataSections.clear();

    // Разделяем файлы на секции и рпеобразовываем в формат запросов по протоколу Modbus
    for (auto pos = 0; pos < data.size(); pos += Modbus::fileSectionLength)
    {
        QByteArray sectionData(data.mid(pos, Modbus::fileSectionLength));
        if (!sectionData.isEmpty())
        {
            requestBuffer.push_back(Modbus::Request {
                Modbus::FunctionCode::WriteFileSection,  //
                fileNum,                                 // номер файла
                section++, false,                        // номер секции
                data.mid(pos, Modbus::fileSectionLength) //
            });
        }
        else
            break;
    }
    requestBuffer.back().isLastSection = true; // Помечаем последнюю секцию
    emit totalWritingBytes(requestBuffer.size() * 10 + data.size());

    // Проверяем первую секцию
    if (!requestBuffer.front().isLastSection)
        emit writingLongData();

    // Преобразовываем элементы из буфера в готовые команды (формат QByteArray)
    std::transform(requestBuffer.cbegin(), requestBuffer.cend(), std::back_inserter(m_longDataSections), //
        [this](const Modbus::Request &request) -> QByteArray {
            return createADU(request); //
        });
    return getNextDataSection();
}

Modbus::Request ModbusRequestParser::convertUserValuesToRequest(const QVariantList &valuesList) const noexcept
{
    Q_ASSERT(valuesList.first().canConvert<DataTypes::FloatStruct>());
    // search for minimum sig addr
    QByteArray sigArray;
    quint16 min_addr = 0xffff;
    for (auto &item : valuesList)
    {
        auto floatData = item.value<DataTypes::FloatStruct>();
        if (floatData.sigAdr < min_addr)
            min_addr = floatData.sigAdr;
        // now write floats to the out sigArray
        sigArray.push_back(helper::packRegister(floatData.sigVal));
    }

    return Modbus::Request {
        Modbus::FunctionCode::WriteMultipleRegisters, //
        static_cast<quint16>(min_addr),               //
        static_cast<quint16>(valuesList.size() * 2),  // количество регистров типа int16
        false, sigArray                               //
    };
}

} // namespace Interface
