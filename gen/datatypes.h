#ifndef DATATYPES_H
#define DATATYPES_H

#include <QVariant>

namespace DataTypes
{
enum SignalTypes
{
    BitString,
    FloatWithTime,
    Float,
    SinglePointWithTime,
    ByteArray,
    File,
    ConfParameter,
    ConfParameterList,
    Block,
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
}; // первое - номера сигналов, второе - их значения ("" ~ недостоверное
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

struct FileStruct
{
    FileStruct() = default;
    ~FileStruct()
    {
    }
    FileStruct(const quint32 num, const QByteArray &file) : filenum(num), filedata(file)
    {
    }
    FileStruct(const FileStruct &source) : FileStruct(source.filenum, source.filedata)
    {
    }
    FileStruct &operator=(const FileStruct &source)
    {
        filenum = source.filenum;
        filedata = source.filedata;
        return *this;
    }
    FileStruct(FileStruct &&rhs) noexcept : filenum(rhs.filenum), filedata(rhs.filedata)
    {
        rhs.filenum = NULL;
        rhs.filedata = nullptr;
    }
    FileStruct operator=(FileStruct &&rhs) noexcept
    {
        if (this != &rhs)
        {
            filenum = rhs.filenum;
            filedata = rhs.filedata;
            rhs.filenum = NULL;
            rhs.filedata = nullptr;
        }
        return *this;
    }
    quint32 filenum;
    QByteArray filedata;
};

struct ConfParameterStruct
{
    quint32 ID;
    QByteArray data;
};

struct BlockStruct
{
    quint32 ID;
    QByteArray data;
};

typedef QList<ConfParameterStruct> ConfParametersListStruct;

// struct ConfParametersListStruct
//{
//    QList<ConfParameterStruct> parlist;
//};

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

struct GeneralResponseStruct
{
    GeneralResponseTypes type;
    quint64 data;
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
    QUSB_ReqTuningCoef,
    QUSB_WriteTuningCoef,
    QUSB_ReqBlkDataA,
    QUSB_ReqBlkDataTech,
    QUSB_WriteBlkDataTech
};

struct Command
{
    Commands cmd;
    quint32 uintarg;
    float flarg;
    QByteArray ba;
};
}

namespace S2DataTypes
{
// S2: Определение типа заголовка
typedef struct
{
    quint16 fname;
    quint16 service;
    quint32 size;
    quint32 crc32;
    quint32 thetime;
} FileHeader;

// S2: Определение типа записи

typedef struct DataRec
{
    quint32 id;
    quint32 num_byte;
    void *thedata;
} DataRec;

typedef QVector<S2DataTypes::DataRec> S2ConfigType;
}

Q_DECLARE_METATYPE(DataTypes::BitStringStruct)
Q_DECLARE_METATYPE(DataTypes::FloatWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::FloatStruct)
Q_DECLARE_METATYPE(DataTypes::SinglePointWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::FileStruct)
Q_DECLARE_METATYPE(DataTypes::ConfParameterStruct)
Q_DECLARE_METATYPE(DataTypes::BlockStruct)
Q_DECLARE_METATYPE(DataTypes::ConfParametersListStruct)
Q_DECLARE_METATYPE(DataTypes::SignalsStruct)
Q_DECLARE_METATYPE(DataTypes::Signal)
Q_DECLARE_METATYPE(DataTypes::GeneralResponseStruct)
Q_DECLARE_METATYPE(Queries::Command)
#endif // DATATYPES_H
