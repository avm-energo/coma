#include "interfaces/parsers/iec104_request_parser.h"

#include <gen/datatypes.h>
#include <s2/filestruct.h>

namespace Interface
{

using namespace Iec104;

Iec104RequestParser::Iec104RequestParser(QObject *parent) : BaseRequestParser(parent)
{
}

void Iec104RequestParser::setBaseStationAddress(const quint16 bsAddress) noexcept
{
    m_baseStationAddress = bsAddress;
}

QByteArray Iec104RequestParser::parse(const CommandStruct &cmd)
{
    m_request.clear();
    switch (cmd.command)
    {
    case Commands::C_ReqBSI:
    case Commands::C_ReqBSIExt:
    {
        // Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, BSIGROUP, 0, {} };
        break;
    }
    case Commands::C_ReqStartup:
    {
        // Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, STARTUPGROUP, 0, {} };
        break;
    }
    case Commands::C_ReqAlarms:
    {
        // Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, ALARMGROUP, 0, {} };
        break;
    }
    case Commands::C_ReqFloats:
    {
        // Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, MAINFLOATGROUP, 0, {} };
        break;
    }
    case Commands::C_ReqBitStrings:
    {
        // Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, MAINBITSTRINGGROUP, 0, {} };
        break;
    }
    case Commands::C_ReqTime:
    {
        // Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, TIMEGROUP, 0, {} };
        break;
    }
    case Commands::C_WriteTime:
    {
        // Commands104::CommandStruct inp { Commands104::CM104_COM51, time, 0, {} };
        break;
    }
    case Commands::C_ReqFile:
    {
        [[maybe_unused]] auto filenum = static_cast<S2::FilesEnum>(cmd.arg1.value<quint32>());
        [[maybe_unused]] auto format = static_cast<DataTypes::FileFormat>(cmd.arg2.value<quint32>());
        // Datatypes::FileFormat format
        // auto cmd104 = (format) ? Commands104::CM104_REQCONFIGFILE : Commands104::CM104_REQFILE;
        // Commands104::CommandStruct inp { cmd104, filenum, 0, {} };
        break;
    }
    case Commands::C_WriteFile:
    {
        [[maybe_unused]] auto filenum = static_cast<S2::FilesEnum>(cmd.arg1.value<quint32>());
        [[maybe_unused]] auto file = cmd.arg2.value<QByteArray>();
        // Commands104::CommandStruct inp { Commands104::CM104_WRITEFILE, filenum, 0, true, file };
        break;
    }
    case Commands::C_WriteUserValues:
    {
        // Q_ASSERT(item.canConvert<DataTypes::FloatStruct>());
        // if (!item.canConvert<DataTypes::FloatStruct>())
        //     return;
        // DataTypes::FloatStruct flstr = item.value<DataTypes::FloatStruct>();
        // Commands104::CommandStruct inp { Commands104::CM104_COM50, flstr.sigAdr, flstr.sigVal, {} };
        break;
    }
    // "WS" commands
    case Commands::C_StartWorkingChannel:
    case Commands::C_EraseJournals:
    case Commands::C_SetStartupValues:
    case Commands::C_SetStartupPhaseA:
    case Commands::C_SetStartupPhaseB:
    case Commands::C_SetStartupPhaseC:
    case Commands::C_ClearStartupUnbounced:
    case Commands::C_SetTransOff:
    case Commands::C_ClearStartupError:
    case Commands::C_ClearStartupValues:
    case Commands::C_SetStartupUnbounced:
    {
        auto search = s_wsCmdMap.find(cmd.command);
        if (search != s_wsCmdMap.cend())
        {
            [[maybe_unused]] auto value = search->second; // auto iec104cmd = get104Command(cmd);
            // Commands104::CommandStruct inp { Commands104::CM104_COM45, iec104cmd, 0, true, {} };
        }
        break;
    }
    default:
        qCritical() << "Undefined command: " << cmd.command;
    }
    return m_request;
}

QByteArray Iec104RequestParser::createGroupRequest([[maybe_unused]] const quint32 groupNum)
{
    return QByteArray {};
}

QByteArray createASDUPrefix(const MessageDataType type, const quint32 address)
{
    QByteArray asdu;
    asdu.append(std_ext::to_underlying(type));
    asdu.append(QByteArrayLiteral("\x01\x06\x00"));
    // asdu.append(m_baseAdrLow);
    // asdu.append(m_baseAdrHigh);
    asdu.append(address);
    asdu.append(address >> 8);
    asdu.append(address >> 16);
    return asdu;
}

QByteArray Iec104RequestParser::getNextContinueCommand() noexcept
{
    return QByteArray {};
};

} // namespace Interface
