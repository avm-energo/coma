#pragma once

#include "interfaces/parsers/protocom_request_parser.h"

#include <interfaces/connection.h>

namespace Interface
{

const std::map<Interface::Commands, Proto::Commands> ProtocomRequestParser::s_protoCmdMap {
    { Commands::C_ReqTime, Proto::ReadTime },                      // 12
    { Commands::C_ReqBSI, Proto::ReadBlkStartInfo },               // 12
    { Commands::C_ReqBSIExt, Proto::ReadBlkStartInfoExt },         // 12
    { Commands::C_StartFirmwareUpgrade, Proto::WriteUpgrade },     // 12
    { Commands::C_SetNewConfiguration, Proto::WriteBlkTech },      // 12
    { Commands::C_WriteUserValues, Proto::WriteBlkData },          // 12
    { Commands::C_EraseJournals, Proto::EraseTech },               // 12
    { Commands::C_ReqProgress, Proto::ReadProgress },              // 12
    { Commands::C_EraseTechBlock, Proto::EraseTech },              // 12
    { Commands::C_Test, Proto::Test },                             // 12
    { Commands::C_WriteSingleCommand, Proto::WriteSingleCommand }, // 12
    { Commands::C_ReqTuningCoef, Proto::ReadBlkAC },               // 12
    { Commands::C_WriteTuningCoef, Proto::WriteBlkAC },            // 12
    { Commands::C_ReqBlkData, Proto::ReadBlkData },                // 12
    { Commands::C_ReqBlkDataA, Proto::ReadBlkDataA },              // 12
    { Commands::C_ReqBlkDataTech, Proto::ReadBlkTech },            // 12
    { Commands::C_ReqOscInfo, Proto::ReadBlkTech },                // 12
    { Commands::C_WriteBlkDataTech, Proto::WriteBlkTech },         // 12
    { Commands::C_Reboot, Proto::WriteBlkCmd },                    // 12
    { Commands::C_GetMode, Proto::ReadMode },                      // 12
    { Commands::C_SetMode, Proto::WriteMode },                     // 12
    { Commands::C_WriteHardware, Proto::WriteHardware }            // 12
};

ProtocomRequestParser::ProtocomRequestParser(QObject *parent) : BaseRequestParser(parent)
{
}

QByteArray ProtocomRequestParser::parse(const CommandStruct &cmd)
{
    m_command.clear();

    switch (cmd.command)
    {
    // commands requesting regs with addresses ("fake" read regs commands)
    case Commands::C_ReqAlarms:
    case Commands::C_ReqStartup:
    case Commands::C_ReqFloats:
    case Commands::C_ReqBitStrings:
    {
        quint8 block = blockByReg(cmd.arg1.toUInt());
        m_command = prepareBlock(Proto::Commands::ReadBlkData, StdFunc::toByteArray(block));
        break;
    }
    // commands without any arguments
    case Commands::C_ReqBSI:
    case Commands::C_ReqBSIExt:
    case Commands::C_ReqTime:
    case Commands::C_StartFirmwareUpgrade:
    case Commands::C_ReqProgress:
    case Commands::C_GetMode:
    {
        if (isSupportedCommand(cmd.command))
            m_command = prepareBlock(s_protoCmdMap.at(cmd.command), QByteArray());
        break;
    }
    // commands with only one uint8 parameter (blocknum or smth similar)
    case Commands::C_EraseTechBlock:
    case Commands::C_EraseJournals:
    case Commands::C_SetMode:
    case Commands::C_Reboot:
    case Commands::C_ReqTuningCoef:
    case Commands::C_ReqBlkData:
    case Commands::C_ReqBlkDataA:
    case Commands::C_ReqBlkDataTech:
    case Commands::C_ReqOscInfo:
    case Commands::C_Test:
    {
        if (isSupportedCommand(cmd.command))
            m_command = prepareBlock(s_protoCmdMap.at(cmd.command), StdFunc::toByteArray(cmd.arg1.value<quint8>()));
        break;
    }
    // file request: known file types should be download from disk and others must be taken from module by Protocom,
    // arg1 - file number
    case Commands::C_ReqFile:
    {
        using namespace S2;
        FilesEnum filetype = cmd.arg1.value<FilesEnum>();
        switch (filetype)
        {
        case FilesEnum::JourSys:
        case FilesEnum::JourWork:
        case FilesEnum::JourMeas:
            /// TODO: Exceptional situation
            // processFileFromDisk(filetype);
            break;
        default:
            m_command = prepareBlock(Proto::Commands::ReadFile, StdFunc::toByteArray(cmd.arg1.value<quint16>()));
            break;
        }
        break;
    }
    // commands with one bytearray argument arg2
    case Commands::C_WriteFile:
    {
        if (cmd.arg2.canConvert<QByteArray>())
        {
            /// TODO
            // writeBlock(Proto::Commands::WriteFile, cmd.arg2.value<QByteArray>());
        }
        break;
    }
    // write time command with different behaviour under different OS's
    case Commands::C_WriteTime:
    {
        QByteArray tmpba;
#ifdef Q_OS_LINUX
        if (cmd.arg1.canConvert<timespec>())
        {
            timespec time = cmd.arg1.value<timespec>();
            tmpba.push_back(StdFunc::toByteArray(quint32(time.tv_sec)));
            tmpba.push_back(StdFunc::toByteArray(quint32(time.tv_nsec)));
        }
        else
#endif
        {
            tmpba = StdFunc::toByteArray(cmd.arg1.value<quint32>());
        }
        m_command = prepareBlock(Proto::Commands::WriteTime, tmpba);
        break;
    }
    // block write, arg1 is BlockStruct of one quint32 (block ID) and one QByteArray (block contents)
    case Commands::C_WriteHardware:
    case Commands::C_WriteBlkDataTech:
    case Commands::C_SetNewConfiguration:
    case Commands::C_WriteTuningCoef:
    {
        if (cmd.arg1.canConvert<DataTypes::BlockStruct>())
        {
            DataTypes::BlockStruct bs = cmd.arg1.value<DataTypes::BlockStruct>();
            m_command = StdFunc::toByteArray(static_cast<quint8>(bs.ID)); // сужающий каст
            m_command.append(bs.data);
            /// TODO
            // writeBlock(protoCommandMap.value(cmdStr.command), ba);
        }
        break;
    }
    // QVariantList write
    case Commands::C_WriteUserValues:
    {
        if (cmd.arg1.canConvert<QVariantList>())
        {
            QVariantList vl = cmd.arg1.value<QVariantList>();
            const quint16 start_addr = vl.first().value<DataTypes::FloatStruct>().sigAdr;
            const auto blockNum = static_cast<quint8>(blockByReg(start_addr)); // сужающий каст
            m_command = StdFunc::toByteArray(blockNum);
            for (const auto &i : vl)
            {
                const float value = i.value<DataTypes::FloatStruct>().sigVal;
                m_command.append(StdFunc::toByteArray(value));
            }
            /// TODO
            // writeBlock(protoCommandMap.value(cmdStr.command), ba);
        }
        break;
    }
    // WS Commands
    case Commands::C_WriteSingleCommand:
    {
        if (cmd.arg1.canConvert<DataTypes::SingleCommand>())
        {
            DataTypes::SingleCommand scmd = cmd.arg1.value<DataTypes::SingleCommand>();
            auto tmpba = StdFunc::toByteArray(scmd.addr) + StdFunc::toByteArray(scmd.value);
            m_command = prepareBlock(Proto::WriteSingleCommand, tmpba);
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
        /// TODO
        //        uint24 converted(WSCommandMap[cmdStr.command]);
        //        ba = StdFunc::toByteArray(converted);
        //        ba.append(StdFunc::toByteArray(cmdStr.arg1.value<quint8>()));
        //        ba = prepareBlock(Proto::WriteSingleCommand, ba);
        //        emit sendDataToInterface(ba);
        break;
    }
    default:
        qDebug() << "There's no such command";
    }
    return m_command;
}

quint16 ProtocomRequestParser::blockByReg(const quint32 regAddr)
{
    return Connection::iface()->settings()->dictionary().value(regAddr).block.value<quint16>();
}

void ProtocomRequestParser::appendInt16(QByteArray &ba, quint16 data)
{
    ba.append(static_cast<char>(data % 0x100));
    ba.append(static_cast<char>(data / 0x100));
}

QByteArray ProtocomRequestParser::prepareBlock(
    const Proto::Commands cmd, const QByteArray &data, Proto::Starters startByte)
{
    QByteArray ba;
    ba.append(startByte);
    ba.append(cmd);
    appendInt16(ba, data.size());

    if (!data.isEmpty())
        ba.append(data);
    return ba;
}

bool ProtocomRequestParser::isSupportedCommand(Commands command) noexcept
{
    const auto search = s_protoCmdMap.find(command);
    return search != s_protoCmdMap.cend();
}

} // namespace Interface
