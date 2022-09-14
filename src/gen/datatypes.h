#ifndef DATATYPES_H
#define DATATYPES_H

#include "uint24.h"

#include <QVariant>

#ifdef __linux__
#include <ctime>
Q_DECLARE_METATYPE(timespec);
#endif

namespace DataTypes
{

enum DataBlockTypes
{
    BacBlock,
    BdaBlock,
    BdBlock,
    BciBlock
};

enum SignalTypes
{
    Float,
    BitString,
    FloatWithTime,
    SinglePointWithTime,
    ByteArray,
    File,
    DataRecVList,
    Block,
    OscillogramInfo,
    SwitchJournalInfo,
#ifdef __linux
    Timespec,
#endif
    GeneralResponse
};

enum GeneralResponseTypes
{
    Ok,
    Error,
    DataSize,
    DataCount
};

struct BitStringStruct
{
    //    quint8 sigAdr[3];
    quint32 sigAdr;
    quint32 sigVal;
    quint8 sigQuality;
};
// первое - номера сигналов, второе - их значения ("" ~ недостоверное
// значение), третье - метка времени

struct FloatWithTimeStruct
{
    quint32 sigAdr;
    float sigVal;
    quint8 sigQuality;
    quint64 CP56Time;
};

struct FloatStruct
{
    quint32 sigAdr;
    float sigVal;
};

struct SinglePointWithTimeStruct
{
    quint32 sigAdr;
    quint8 sigVal;
    quint64 CP56Time;
};

struct BlockStruct
{
    quint32 ID;
    QByteArray data;
};

struct GeneralResponseStruct
{
    GeneralResponseTypes type;
    quint64 data;
};

typedef BlockStruct S2Record;
typedef BlockStruct HardwareStruct;
typedef QList<S2Record> S2FilePack;

struct SignalsStruct
{
    SignalTypes type;
    QVariant data;
};

struct Signal
{
    quint16 addr;
    quint16 value;
};

struct SingleCommand
{
    uint24 addr;
    bool value;
};

}

namespace Queries
{
enum Commands
{
    QC_Unknown, // NOTE Temporary warn command
    QC_SetNewConfiguration,
    QC_StartFirmwareUpgrade,
    QC_StartWorkingChannel,
    QC_EraseJournals,
    QC_SetStartupValues,
    QC_ClearStartupValues,
    QC_Command50,
    QC_Test,
    QC_EraseTechBlock,
    QC_WriteHiddenBlock,
    QC_WriteUserValues,
    QC_ReqAlarms,
    QC_ReqFloats,
    QC_ReqBitStrings,
    QC_WriteSingleCommand,
    QUSB_ReqTuningCoef,
    QUSB_WriteTuningCoef,
    QUSB_WriteBlkData,
    QUSB_ReqBlkData,
    QUSB_ReqBlkDataA,
    QUSB_ReqBlkDataTech,
    QUSB_WriteBlkDataTech,
    QUSB_Reboot,
    QUSB_ReqOscInfo,
    QUSB_SetMode,
    QUSB_GetMode,
    QUSB_WriteHardware
};

struct Command
{
    Commands cmd;
    quint32 uintarg;
    float flarg;
    QByteArray ba;
};

enum FileFormat : quint32
{
    Binary = 0,
    DefaultS2 = 1,
    CustomS2
};
}

Q_DECLARE_METATYPE(DataTypes::SingleCommand)
Q_DECLARE_METATYPE(uint24)
Q_DECLARE_METATYPE(DataTypes::BitStringStruct)
Q_DECLARE_METATYPE(DataTypes::FloatWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::FloatStruct)
Q_DECLARE_METATYPE(DataTypes::SinglePointWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::BlockStruct)
Q_DECLARE_METATYPE(DataTypes::S2FilePack)
Q_DECLARE_METATYPE(DataTypes::SignalsStruct)
Q_DECLARE_METATYPE(DataTypes::Signal)
Q_DECLARE_METATYPE(DataTypes::GeneralResponseStruct)
Q_DECLARE_METATYPE(Queries::Command)

#endif // DATATYPES_H
