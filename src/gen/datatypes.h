#ifndef DATATYPES_H
#define DATATYPES_H

#include "datamanager/datatypeimpl.h"
#include "uint24.h"

#include <QVariant>

#ifdef __linux__
#include <ctime>

class TimeSpec : DataTypeImpl<TimeSpec>
{
public:
    TimeSpec() : DataTypeImpl(this)
    {
    }

    timespec tSpec;
};

Q_DECLARE_METATYPE(TimeSpec);
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
    BitString,
    FloatWithTime,
    Float,
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

class BitStringStruct : DataTypeImpl<BitStringStruct>
{
public:
    BitStringStruct() : DataTypeImpl(this)
    {
    }

    //    quint8 sigAdr[3];
    quint32 sigAdr;
    quint32 sigVal;
    quint8 sigQuality;

signals:
    void DataTypeReceived(const BitStringStruct &);
};
// первое - номера сигналов, второе - их значения ("" ~ недостоверное
// значение), третье - метка времени

class FloatWithTimeStruct : DataTypeImpl<FloatWithTimeStruct>
{
public:
    FloatWithTimeStruct() : DataTypeImpl(this)
    {
    }

    quint32 sigAdr;
    float sigVal;
    quint8 sigQuality;
    quint64 CP56Time;

signals:
    void DataTypeReceived(const FloatWithTimeStruct &);
};

class FloatStruct : DataTypeImpl<FloatStruct>
{
public:
    FloatStruct() : DataTypeImpl(this)
    {
    }

    quint32 sigAdr;
    float sigVal;

signals:
    void DataTypeReceived(const FloatStruct &);
};

class SinglePointWithTimeStruct : DataTypeImpl<SinglePointWithTimeStruct>
{
public:
    SinglePointWithTimeStruct() : DataTypeImpl(this)
    {
    }

    quint32 sigAdr;
    quint8 sigVal;
    quint64 CP56Time;

signals:
    void DataTypeReceived(const SinglePointWithTimeStruct &);
};

class BlockStruct : DataTypeImpl<BlockStruct>
{
public:
    BlockStruct() : DataTypeImpl(this)
    {
    }

    quint32 ID;
    QByteArray data;

signals:
    void DataTypeReceived(const BlockStruct &);
};

class GeneralResponseStruct : DataTypeImpl<GeneralResponseStruct>
{
public:
    GeneralResponseStruct() : DataTypeImpl(this)
    {
    }

    GeneralResponseTypes type;
    quint64 data;

signals:
    void DataTypeReceived(const GeneralResponseStruct &);
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

struct Alarm
{
    std::bitset<128> flags;
    quint32 startAddr;
    QStringList desc;
    QString name;
    QString color;
};

struct JournalDesc
{
    quint32 id;
    QStringList header;
    QStringList desc;
    QString name;
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
