#pragma once

#include <QVariant>

namespace Interface
{

enum State
{
    Connect,
    Reconnect,
    Disconnect,
    Run
};

enum Direction
{
    NoDirection,
    ToDevice,
    FromDevice
};

enum class Commands
{
    C_ReqStartup,
    C_ReqBSI,
    C_ReqBSIExt,
    C_ReqAlarms,
    C_ReqFile,
    C_WriteFile,
    C_ReqTime,
    C_WriteTime,
    C_ReqFloats,
    C_ReqBitStrings,
    C_ReqProgress,
    C_SetNewConfiguration,
    C_StartFirmwareUpgrade,
    C_EraseJournals,

    C_StartWorkingChannel,
    C_SetStartupValues,
    C_SetStartupPhaseA,
    C_SetStartupPhaseB,
    C_SetStartupPhaseC,
    C_SetStartupUnbounced,
    C_SetTransOff,
    C_ClearStartupValues,
    C_ClearStartupUnbounced,
    C_ClearStartupError,

    C_Test,
    C_EraseTechBlock,
    C_WriteHiddenBlock,
    C_WriteUserValues,
    C_WriteSingleCommand,
    C_ReqTuningCoef,
    C_WriteTuningCoef,
    C_WriteBlkData,
    C_ReqBlkData,
    C_ReqBlkDataA,
    C_ReqBlkDataTech,
    C_WriteBlkDataTech,
    C_Reboot,
    C_ReqOscInfo,
    C_SetMode,
    C_GetMode,
    C_WriteHardware
};

enum CommandRegisters
{
    StartWorkingChannel = 803,  ///< Старт рабочего канала
    SetStartupValues = 900,     ///< Задать начальные значения по всем фазам
    SetStartupPhaseA = 901,     ///< Задать начальные значения по фазе A
    SetStartupPhaseB = 902,     ///< Задать начальные значения по фазе B
    SetStartupPhaseC = 903,     ///< Задать начальные значения по фазе C
    SetStartupUnbounced = 904,  ///< Задать начальные значения по току небаланса
    ClearStartupValues = 905,   ///< Сбросить начальные значения по всем фазам
    ClearStartupSetError = 906, ///< Сбросить ошибку задания начальных значений
    SetTransOff = 907,          ///< Послать команду "Трансфоратор отключён"
    ClearStartupUnbounced = 908 ///< Сбросить начальное значение тока небаланса
};

enum TechBlocks
{
    T_Oscillogram = 0x01,
    T_GeneralEvent = 0x02,
    T_TechEvent = 0x03,
    T_SwitchJournal = 0x04,
    T_WorkArchive = 0x05
};

struct CommandStruct
{
    Commands command;
    QVariant arg1; // reqFile, writeFile: id, reqStartup: sigAddr, WriteTime: time, WriteCommand: command
    QVariant arg2; // reqFile: format, reqStartup: sigCount, WriteFile: &bytearray, WriteCommand: value
};

} // namespace Interface
