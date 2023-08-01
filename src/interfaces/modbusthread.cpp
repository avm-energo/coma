#include "modbusthread.h"

#include "baseinterface.h"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <QtEndian>
#include <algorithm>
#include <gen/datamanager/datamanager.h>
#include <gen/helper.h>
#include <gen/pch.h>
#include <gen/stdfunc.h>
#include <gen/utils/crc16.h>

constexpr auto RECONNECTTIME = 10000;

using namespace Interface;

ModbusThread::ModbusThread(QObject *parent) : BaseInterfaceThread(parent)
{
}

ModbusThread::~ModbusThread()
{
}

void ModbusThread::parseRequest(const CommandStruct &cmdStr)
{
    switch (cmdStr.command)
    {
    // commands requesting regs with addresses ("fake" read regs commands)
    case Commands::C_ReqAlarms:
    {
        MBS::CommandStruct inp {
            MBS::Commands::ReadCoils,     //
            cmdStr.arg1.value<quint16>(), // нехорошо, т.к. кладём туда quint32
            cmdStr.arg2.value<quint16>(), //
            {}                            //
        };
        readCoils(inp);
        break;
    }
    case Commands::C_ReqStartup:
    case Commands::C_ReqFloats:
    case Commands::C_ReqBitStrings:
    case Commands::C_ReqBSI:
    case Commands::C_ReqBSIExt:
    {
        const quint8 count = (cmdStr.arg2.value<quint16>() * 2);   // startup registers are float (2 ints long)
        MBS::CommandStruct inp { MBS::Commands::ReadInputRegister, //
            cmdStr.arg1.value<quint16>(), // нехорошо, т.к. кладём туда quint32
            count,                        //
            {} };
        readRegisters(inp);
        break;
    }

    // commands without any arguments
    case Commands::C_ReqTime:
    {
        MBS::CommandStruct inp {
            MBS::Commands::ReadHoldingRegisters, //
            Regs::timeReg,                       //
            2,                                   //
            {}                                   //
        };
        readRegisters(inp);
        break;
    }

    // arg1 - file number
    case Commands::C_ReqFile:
    {
        MBS::CommandStruct inp {
            MBS::Commands::ReadFileSection, //
            cmdStr.arg1.value<quint16>(), // номер файла - нехорошо, т.к. кладём туда quint32
            0,                            // номер секции
            StdFunc::ArrayFromNumber(MBS::FileSectionLength) //
        };
        m_fileData.clear();
        m_fileSectionNum = 0;
        readRegisters(inp);
        break;
    }

    // commands with one bytearray argument arg2
    case Commands::C_WriteFile:
    {
        m_fileData = cmdStr.arg2.value<QByteArray>();
        setProgressRange(m_fileData.size());
        m_sentBytesCount = 0;
        m_fileSectionNum = 0;
        writeFile(cmdStr.arg1.value<quint16>());
        break;
    }

    case Commands::C_StartFirmwareUpgrade:
    {
        QByteArray value = packReg(quint16(1));
        constexpr quint16 firmwareModbusAddr = 802;
        MBS::CommandStruct command {
            MBS::Commands::WriteMultipleRegisters, //
            firmwareModbusAddr,                    //
            1,                                     //
            value                                  //
        };
        writeMultipleRegisters(command);
        break;
    }

    // write time command with different behaviour under different OS's
    case Commands::C_WriteTime:
    {
        QByteArray timeArray = packReg(cmdStr.arg1.value<quint32>());
        MBS::CommandStruct inp {
            MBS::Commands::WriteMultipleRegisters, //
            Regs::timeReg,                         //
            2,                                     //
            timeArray                              //
        };
        writeMultipleRegisters(inp);
        break;
    }

    // QVariantList write
    case Commands::C_WriteUserValues:
    {
        if (cmdStr.arg1.canConvert<QVariantList>())
        {
            QVariantList list = cmdStr.arg1.value<QVariantList>();
            Q_ASSERT(list.first().canConvert<DataTypes::FloatStruct>());

            quint16 some_addr = list.first().value<DataTypes::FloatStruct>().sigAdr;
            // search for minimum sig addr
            QByteArray sigArray;
            quint16 min_addr = some_addr;
            for (auto &i : list)
            {
                auto flstr = i.value<DataTypes::FloatStruct>();
                if (flstr.sigAdr < min_addr)
                    min_addr = flstr.sigAdr;
                // now write floats to the out sigArray
                sigArray.push_back(packReg(flstr.sigVal));
            }

            MBS::CommandStruct inp {
                MBS::Commands::WriteMultipleRegisters, //
                static_cast<quint16>(min_addr),        //
                static_cast<quint16>(list.size() * 2), // количество регистров типа int16
                sigArray                               //
            };
            writeMultipleRegisters(inp);
        }
        break;
    }

    // WS Commands
    case Commands::C_WriteSingleCommand:
    {
        if (cmdStr.arg1.canConvert<DataTypes::SingleCommand>())
        {
            DataTypes::SingleCommand scmd = cmdStr.arg1.value<DataTypes::SingleCommand>();
            MBS::CommandStruct inp {
                MBS::Commands::WriteMultipleRegisters, //
                scmd.addr.toU16(),                     //
                1,                                     // количество регистров типа int16
                StdFunc::ArrayFromNumber(scmd.value)   //
            };
            writeMultipleRegisters(inp);
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
        auto cmdAddr = WSCommandMap[cmdStr.command];
        if (cmdAddr)
        {
            auto value = static_cast<quint16>(cmdStr.arg1.value<quint16>());
            MBS::CommandStruct inp {
                MBS::Commands::WriteMultipleRegisters, //
                static_cast<quint16>(cmdAddr),         //
                1,                                     //
                packReg(value)                         //
            };
            writeMultipleRegisters(inp);
        }
        break;
    }

    default:
    {
        qDebug() << "There's no such command";
    }
    }
}

void ModbusThread::parseResponse()
{
    using namespace MBS;
    switch (m_currentCommand.command)
    {
    case Commands::C_ReqFloats:
    case Commands::C_ReqStartup:
        processFloatSignals();
        break;
    case Commands::C_ReqBitStrings:
    case Commands::C_ReqBSI:
    case Commands::C_ReqBSIExt:
    case Commands::C_ReqTime:
        processIntegerSignals();
        break;
    case Commands::C_ReqAlarms:
        processSinglePointSignals();
        break;
    case Commands::C_ReqFile:
    {
        if (processReadFile())
        {
            MBS::CommandStruct inp { MBS::Commands::ReadFileSection, //
                m_currentCommand.arg1.value<quint16>(), // номер файла - нехорошо, т.к. кладём туда quint32
                ++m_fileSectionNum,                     // номер секции
                StdFunc::ArrayFromNumber(MBS::FileSectionLength) };
            readRegisters(inp);
            return; // продолжаем, пока не получим весь файл
        }
        else // получили весь файл
        {
            quint32 addr = m_currentCommand.arg1.toUInt();
            quint32 count = m_currentCommand.arg2.toUInt();
            FilePostpone(m_fileData, S2::FilesEnum(addr), FileFormat(count));
        }
        break;
    }
    case Commands::C_WriteFile:
    {
        // Q_ASSERT(m_readData.size() == 8);
        // m_sentBytesCount += quint8(m_readData[7]);
        // setProgressCount(m_sentBytesCount);
        if (writeFile(m_currentCommand.arg1.value<quint16>())) // посылаем следующую секцию или возвращаем false
            return;
        DataTypes::GeneralResponseStruct grs;
        grs.type = DataTypes::GeneralResponseTypes::Ok;
        DataManager::GetInstance().addSignalToOutList(grs);
        break;
    }
    default:
        processCommandResponse();
        break;
    }
    finishCommand();
}

void ModbusThread::setDeviceAddress(quint8 adr)
{
    m_deviceAddress = adr;
}

void ModbusThread::processReadBytes(QByteArray ba)
{
    m_readData.append(ba);
    if (m_readData.size() >= 2)
    {
        quint8 receivedCommand = m_readData.at(1);
        if (receivedCommand & 0x80)
        {
            m_log->error("Modbus error response: " + m_readData.toHex());
            qCritical() << Error::ReadError << metaObject()->className();
            finishCommand();
            return;
        }
    }
    if (m_currentCommand.command == C_ReqFile)
    {
        if (m_readData.size() >= 8)                // [8] = section length
            m_bytesToReceive = m_readData[7] + 10; // дополнительные данные
        else
            return; // не получили ещё длину, рано продолжать
    }
    if (m_readData.size() >= m_bytesToReceive)
    {
        m_log->info("<- " + m_readData.toHex());
        int rdsize = m_readData.size();

        quint16 receivedCRC = (quint8(m_readData[rdsize - 2]) << 8) | quint8(m_readData[rdsize - 1]);
        m_readData.chop(2);
        utils::CRC16 calculatedCRC(m_readData);
        if (calculatedCRC != receivedCRC)
        {
            m_log->error("Crc error");
            qCritical() << Error::CrcError << metaObject()->className();
            m_readData.clear();
        }
        m_parsingDataReady = true;
        wakeUp();
    }
}

void ModbusThread::setDelay(quint8 newDelay)
{
    m_delay = newDelay;
}

void ModbusThread::calcCRCAndSend(QByteArray &ba)
{
    utils::CRC16 crc(ba);
    ba.append(crc.toByteArray());
    send(ba);
}

void ModbusThread::send(const QByteArray &ba)
{
    m_readData.clear();
    m_log->info("-> " + ba.toHex());
    emit sendDataToPort(ba);
}

void ModbusThread::processFloatSignals()
{
    if (m_readData.size() < 3)
    {
        m_log->error("Wrong inbuf size");
        qCritical() << Error::SizeError << metaObject()->className();
        return;
    }
    int byteSize = m_readData.data()[2];
    QByteArray ba = m_readData.mid(3);
    if (byteSize > ba.size())
    {
        qCritical() << Error::SizeError << metaObject()->className();
        return;
    }
    for (auto i = 0; i < ba.size(); i += sizeof(float))
    {
        DataTypes::FloatStruct signal;
        signal.sigVal = unpackReg<float>(ba.mid(i, sizeof(float)));
        signal.sigAdr = m_commandSent.adr + i / sizeof(float);
        signal.sigQuality = DataTypes::Quality::Good;
        DataManager::GetInstance().addSignalToOutList(signal);
    }
}

void ModbusThread::processIntegerSignals()
{
    if (m_readData.size() < 3)
    {
        m_log->error("Wrong inbuf size");
        qCritical() << Error::SizeError << metaObject()->className();
        return;
    }
    int byteSize = m_readData.data()[2];
    QByteArray ba = m_readData.mid(3);
    if (byteSize > ba.size())
    {
        qCritical() << Error::SizeError << metaObject()->className();
        return;
    }
    for (auto i = 0; i < ba.size(); i += sizeof(quint32))
    {
        DataTypes::BitStringStruct signal;
        signal.sigVal = unpackReg<quint32>(ba.mid(i, sizeof(quint32)));
        signal.sigAdr = m_commandSent.adr + i / sizeof(quint32);
        signal.sigQuality = DataTypes::Quality::Good;
        DataManager::GetInstance().addSignalToOutList(signal);
    }
}

void ModbusThread::processCommandResponse()
{
    if (m_readData.size() < 3)
    {
        qCritical() << Error::SizeError << metaObject()->className();
        m_log->error("Wrong inbuf size");
        return;
    }
    // ?
    int byteSize = m_readData.at(2);
    QByteArray ba = m_readData.mid(3);
    if (byteSize > ba.size())
    {
        qCritical() << Error::SizeError << metaObject()->className();
        m_log->error("Wrong byte size in response");
        return;
    }
    DataTypes::GeneralResponseStruct grs;
    grs.type = DataTypes::GeneralResponseTypes::Ok;
    DataManager::GetInstance().addSignalToOutList(grs);
}

void ModbusThread::processSinglePointSignals()
{
    DataTypes::SinglePointWithTimeStruct signal;

    if (m_readData.size() < 3)
    {
        qCritical() << Error::SizeError << metaObject()->className();
        return;
    }
    int byteSize = m_readData.data()[2];
    QByteArray ba = m_readData.mid(3);
    if (byteSize > ba.size())
    {
        qCritical() << Error::SizeError << metaObject()->className();
        return;
    }
    for (int i = 0; i < byteSize; ++i)
    {
        quint8 ival = ba.at(i);
        for (int j = 0; j < 8; ++j)
        {
            signal.sigAdr = m_commandSent.adr + i * 8 + j;
            signal.sigVal = ((0x01 << j) & ival) ? 1 : 0;
            signal.sigQuality = DataTypes::Quality::Good;
            DataManager::GetInstance().addSignalToOutList(signal);
        }
    }
}

bool ModbusThread::processReadFile()
{
    if (m_readData.size() < 8) // "шапка" секции
    {
        qCritical() << Error::SizeError << metaObject()->className();
        return false; // ошибка, закончили с файлом
    }
    quint8 isLastSection = m_readData.at(2);
    quint16 fileNum = unpackReg<quint16>(m_readData.mid(3, sizeof(quint16)));
    quint16 secNum = unpackReg<quint16>(m_readData.mid(5, sizeof(quint16)));
    Q_UNUSED(secNum)
    // проверяем, что номер файла тот, что мы ожидаем
    Q_ASSERT(fileNum == m_currentCommand.arg1.value<quint16>());
    // проверка, прочитали ли мы столько байт, сколько заявлено в шапке
    auto size1 = m_readData.at(7);
    auto size2 = m_readData.size() - 8;
    Q_ASSERT(size1 == size2);
    if (m_readData.size() > 8) // если в секции вообще что-то есть, может быть и нулевая
        m_fileData.append(m_readData.mid(8));
    if (isLastSection)
        return false;
    return true;
}

void ModbusThread::readRegisters(MBS::CommandStruct &cms)
{
    m_commandSent = cms;
    QByteArray ba(createReadPDU(cms));
    if (cms.data.size())
        ba.append(cms.data);
    ba = createADU(ba);
    m_bytesToReceive = cms.quantity * 2 + 5; // address, function code, bytes count, crc (2)
    send(ba);
}

void ModbusThread::readCoils(MBS::CommandStruct &cms)
{
    m_commandSent = cms;
    QByteArray ba(createReadPDU(cms));
    ba = createADU(ba);
    auto temp = cms.quantity / 8 + ((cms.quantity % 8) != 0);
    m_bytesToReceive = temp + 5; // address, function code, bytes count, crc (2)
    send(ba);
}

void ModbusThread::writeMultipleRegisters(MBS::CommandStruct &cms)
{
    QByteArray ba;
    setQueryStartBytes(cms, ba);
    ba.append(cms.quantity * 2); // количество байт
    if (cms.data.size())
        ba.append(cms.data);
    m_bytesToReceive = 8; // address, function code, address (2), quantity (2), crc (2)
    calcCRCAndSend(ba);
}

bool ModbusThread::writeFile(quint16 fileNum)
{
    quint8 lastSection = 0; // не последняя секция
    QByteArray ba = m_fileData.mid(0, MBS::FileSectionLength);
    m_sentBytesCount += ba.size();
    if (ba.isEmpty())
        return false;
    m_fileData = m_fileData.mid(ba.size()); // удаляем секцию из массива
    if (m_fileData.isEmpty())
        lastSection = 1;
    ba.prepend(StdFunc::ArrayFromNumber(static_cast<quint8>(ba.size())));
    ba.prepend(StdFunc::ArrayFromNumber(qToBigEndian(m_fileSectionNum++)));
    ba.prepend(StdFunc::ArrayFromNumber(qToBigEndian(fileNum)));
    ba.prepend(StdFunc::ArrayFromNumber(lastSection));
    ba.prepend(MBS::Commands::WriteFileSection);
    ba.prepend(m_deviceAddress);
    m_bytesToReceive = 10; // address, function code, last section, numFile (2), numSection (2), secLength, crc (2)
    calcCRCAndSend(ba);
    setProgressCount(m_sentBytesCount);
    return true;
}

void ModbusThread::setQueryStartBytes(MBS::CommandStruct &cms, QByteArray &ba)
{
    m_commandSent = cms;
    ba.append(m_deviceAddress); // адрес устройства
    ba.append(cms.cmd);         // аналоговый выход
    QByteArray bigEndArray;
    bigEndArray = StdFunc::ArrayFromNumber(qToBigEndian(cms.adr));
    ba.append(bigEndArray);
    bigEndArray = StdFunc::ArrayFromNumber(qToBigEndian(cms.quantity));
    ba.append(bigEndArray);
}

QByteArray ModbusThread::createReadPDU(const MBS::CommandStruct &cms) const
{
    QByteArray ba;
    ba.append(cms.cmd);
    ba.append(StdFunc::ArrayFromNumber(qToBigEndian(cms.adr)));
    ba.append(StdFunc::ArrayFromNumber(qToBigEndian(cms.quantity)));
    return ba;
}

QByteArray ModbusThread::createADU(const QByteArray &pdu) const
{
    QByteArray ba;
    ba.append(m_deviceAddress);
    ba.append(pdu);
    utils::CRC16 crc(ba);
    ba.append(crc.toByteArray());
    return ba;
}
