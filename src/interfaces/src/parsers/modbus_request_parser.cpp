#include "interfaces/parsers/modbus_request_parser.h"

#include <interfaces/connection.h>

namespace helper
{

template <typename T> //
QByteArray packRegister(T value)
{
    QByteArray ba;
    std::array<std::uint8_t, sizeof(T)> valueBytes;
    auto srcBegin = reinterpret_cast<std::uint8_t *>(&value);
    auto srcEnd = srcBegin + sizeof(T);
    auto dstBeign = valueBytes.begin();
    std::copy(srcBegin, srcEnd, dstBeign);
    for (auto it = valueBytes.begin(); it != valueBytes.end(); it = it + 2)
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
            Modbus::FunctionCode::ReadCoils, //
            cmd.arg1.value<quint16>(),       // нехорошо, т.к. кладём туда quint32
            cmd.arg2.value<quint16>(),       //
            {}                               //
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
            count,                                   //
            {}                                       //
        };
        break;
    }
    // writing registers
    case Commands::C_ReqTime:
    {
        request = Modbus::Request {
            Modbus::FunctionCode::ReadHoldingRegisters, //
            Modbus::timeReg,                            //
            2,                                          //
            {}                                          //
        };
        break;
    }
    // request file
    case Commands::C_ReqFile:
    {
        request = Modbus::Request {
            Modbus::FunctionCode::ReadFileSection,          //
            cmd.arg1.value<quint16>(),                      // номер файла
            0,                                              // номер секции
            StdFunc::toByteArray(Modbus::fileSectionLength) //
        };
        // m_fileData.clear();
        // m_fileSectionNum = 0;
        // readRegisters(inp);
        break;
    }
    // write file
    case Commands::C_WriteFile:
    {
        auto fileNum = cmd.arg1.value<quint16>();
        auto fileData = cmd.arg2.value<QByteArray>();
        // setProgressRange(m_fileData.size());
        // m_sentBytesCount = 0;
        // m_fileSectionNum = 0;
        // writeFile();
        break;
    }
    // writing registers
    case Commands::C_StartFirmwareUpgrade:
    {
        auto value = helper::packRegister(quint16(1));
        request = Modbus::Request {
            Modbus::FunctionCode::WriteMultipleRegisters, //
            Modbus::firmwareModbusAddr,                   //
            1,                                            //
            value                                         //
        };
        break;
    }
    // writing registers
    case Commands::C_WriteTime:
    {
        QByteArray timeArray = helper::packRegister(cmd.arg1.value<quint32>());
        request = Modbus::Request {
            Modbus::FunctionCode::WriteMultipleRegisters, //
            Modbus::timeReg,                              //
            2,                                            //
            timeArray                                     //
        };
        break;
    }
    // writing registers
    case Commands::C_WriteUserValues:
    {
        if (cmd.arg1.canConvert<QVariantList>())
        {
            auto list = cmd.arg1.value<QVariantList>();
            Q_ASSERT(list.first().canConvert<DataTypes::FloatStruct>());

            // search for minimum sig addr
            QByteArray sigArray;
            quint16 min_addr = 0xffff;
            for (auto &item : list)
            {
                auto floatData = item.value<DataTypes::FloatStruct>();
                if (floatData.sigAdr < min_addr)
                    min_addr = floatData.sigAdr;
                // now write floats to the out sigArray
                sigArray.push_back(helper::packRegister(floatData.sigVal));
            }

            request = Modbus::Request {
                Modbus::FunctionCode::WriteMultipleRegisters, //
                static_cast<quint16>(min_addr),               //
                static_cast<quint16>(list.size() * 2),        // количество регистров типа int16
                sigArray                                      //
            };
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
                singleCmd.addr,                               //
                1,                                            // количество регистров типа int16
                StdFunc::toByteArray(singleCmd.value)         //
            };
        }
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
                1,                                                //
                value                                             //
            };
        }
        break;
    }
    default:
        qCritical() << "Undefined command: " << QVariant::fromValue(cmd.command).toString();
    }

    return m_request;
}

QByteArray ModbusRequestParser::getNextContinueCommand() noexcept
{
    return {};
};

void ModbusRequestParser::setDeviceAddress(const quint8 deviceAddress) noexcept
{
    m_deviceAddress = deviceAddress;
}

QByteArray ModbusRequestParser::createPDU(const Modbus::Request &request)
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
    case Modbus::FunctionCode::ReadHoldingRegisters:
    case Modbus::FunctionCode::ReadInputRegister:
        bytesToReceive = request.quantity * 2 + 5; // address (1), function code (1), bytes count (1), crc (2)
        break;
    case Modbus::FunctionCode::WriteMultipleRegisters:
        bytesToReceive = 8; // address (1), function code (1), address (2), quantity (2), crc (2)
        if (request.quantity * 2 != request.data.size())
            throw std::runtime_error("Incorrect quantity"); // больно стреляем exception по ногам вызывающей стороны
        if (request.data.size() == 0)
            throw std::runtime_error("Incorrect command data"); // ещё раз стреляем, ещё больнее
        pdu.append(StdFunc::toByteArray(qToBigEndian(request.quantity * 2)));
        pdu.append(request.data);
        break;
    default:
        break;
    }

    emit expectedResponseSize(bytesToReceive);
    return pdu;
}

} // namespace Interface
