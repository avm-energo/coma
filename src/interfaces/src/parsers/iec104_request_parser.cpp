#include "interfaces/parsers/iec104_request_parser.h"

#include <gen/utils/crc16.h>

namespace Interface
{

Iec104RequestParser::Iec104RequestParser(QObject *parent) : BaseRequestParser(parent)
{
}

QByteArray Iec104RequestParser::parse(const CommandStruct &cmd)
{
    m_request.clear();
    switch (cmd.command)
    {
    case Commands::C_ReqAlarms:
    {
        break;
    }
    case Commands::C_ReqStartup:
    case Commands::C_ReqFloats:
    case Commands::C_ReqBitStrings:
    case Commands::C_ReqBSI:
    case Commands::C_ReqBSIExt:
    {
        break;
    }
    case Commands::C_ReqTime:
    {
        break;
    }
    // request file
    case Commands::C_ReqFile:
    {
        break;
    }
    // write file
    case Commands::C_WriteFile:
    {
    }
    // writing registers
    case Commands::C_StartFirmwareUpgrade:
    {
        break;
    }
    // writing registers
    case Commands::C_WriteTime:
    {
        break;
    }
    // writing registers
    case Commands::C_WriteUserValues:
    {
        break;
    }
    // writing registers
    case Commands::C_WriteSingleCommand:
    {
        break;
    }
    // writing registers
    case Commands::C_WriteHardware:
    {
        break;
    }
    // writing registers
    case Commands::C_EnableWritingHardware:
    {
        break;
    }
    // "WS" commands
    case Commands::C_ClearStartupError:
    case Commands::C_EraseJournals:
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
        break;
    }
    default:
        qCritical() << "Undefined command: " << cmd.command;
    }
    return m_request;
}

QByteArray Iec104RequestParser::getNextContinueCommand() noexcept
{
    return QByteArray {};
};

} // namespace Interface
