#ifndef DATATYPES_H
#define DATATYPES_H
#include "datarecv.h"

#include <QDataStream>
#include <QVariant>
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
    ConfParameter,
    ConfParametersList,
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

/*!
    Приложение 3. Номера файлов
        */
enum FilesEnum : quint16
{
    /// Конфигурация
    Config = 1,
    /// Встроенное ПО (Firmware)
    Firmware = 3,
    /// Системный журнал
    JourSys = 4,
    /// Рабочий журнал
    JourWork = 5,
    /// Журнал измерений
    JourMeas = 6,
    /// Журнал переключений
    JourSw = 17,
    /// events journal (12->62)
    JourEv = 18,
    /// oscilloscope info
    FileOsc = 1000
};

struct FileStruct
{
    friend QDataStream &operator<<(QDataStream &stream, const FileStruct &str);
    friend QDataStream &operator>>(QDataStream &stream, FileStruct &str);
    FileStruct() = default;
    ~FileStruct()
    {
    }
    FileStruct(const FilesEnum num, const QByteArray &file) : filenum(num), filedata(file)
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
        // rhs.filenum = 0;
        rhs.filedata = nullptr;
    }
    FileStruct operator=(FileStruct &&rhs) noexcept
    {
        if (this != &rhs)
        {
            filenum = rhs.filenum;
            filedata = rhs.filedata;
            // rhs.filenum = NULL;
            rhs.filedata = nullptr;
        }
        return *this;
    }
    FilesEnum filenum;
    QByteArray filedata;
};

inline QDataStream &operator<<(QDataStream &stream, const FileStruct &str)
{
    stream << str.filenum;
    stream << str.filedata;
    return stream;
}
inline QDataStream &operator>>(QDataStream &stream, FileStruct &str)
{
    stream >> str.filenum;
    stream >> str.filedata;
    return stream;
}

struct ConfParameterStruct
{
    friend QDataStream &operator<<(QDataStream &stream, const ConfParameterStruct &str);
    friend QDataStream &operator>>(QDataStream &stream, ConfParameterStruct &str);
    quint32 ID;
    QByteArray data;
};

inline QDataStream &operator<<(QDataStream &stream, const ConfParameterStruct &str) // сериализуем;
{
    stream << str.ID;
    stream << str.data;
    return stream;
}

inline QDataStream &operator>>(QDataStream &stream, ConfParameterStruct &str) // десериализуем;
{
    stream >> str.ID;
    stream >> str.data;
    return stream;
}

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

struct Alarm
{
    quint32 flags;
    quint32 startAddr;
    QStringList desc;
    QString name;
    QString color;
};
struct Journal
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
    QUSB_ReqTuningCoef,
    QUSB_WriteTuningCoef,
    QUSB_ReqBlkData,
    QUSB_ReqBlkDataA,
    QUSB_ReqBlkDataTech,
    QUSB_WriteBlkDataTech,
    QUSB_SetMode, // SMode (0x43) - not tested yet
    QUSB_GetMode  // GMode (0x28) - not tested yet
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
/// Заголовок файла (Прил. 1)
struct FileHeader
{
    quint16 fname;
    quint16 service;
    quint32 size;
    quint32 crc32;
    quint32 thetime;
};

// S2: Определение типа записи

struct DataRec;
// struct DataRec
//{
//    quint32 id;
//    quint32 num_byte;
//    void *thedata;
//};
// inline bool is_same(const S2DataTypes::DataRec &lhs, const S2DataTypes::DataRec &rhs)
//{
//    if ((lhs.id == rhs.id) && (lhs.num_byte == rhs.num_byte))
//        return !memcmp(lhs.thedata, rhs.thedata, lhs.num_byte);
//    else
//        return false;
//}

struct DataRecHeader
{
    // id
    quint32 id;
    // количество байт в TypeTheData
    quint32 NumByte;
};

/// Тип группы плат
struct DataRecT
{
    // заголовок записи
    DataRecHeader TypeHeader;
    quint8 TypeTheData[4];
    quint8 VerPO[4];
};
/// Файл ВПО в формате BIN
struct DataRecF
{
    /// заголовок записи
    DataRecHeader FileDatHeader;
    QByteArray Data;
};

#pragma pack(push, 1)
struct File_struct
{
    FileHeader File_xxx_header;
    DataRecT Type;
    DataRecF File;
    // заголовок пустой записи
    DataRecHeader void_recHeader;
};
#pragma pack(pop)
typedef QVector<S2DataTypes::DataRec> S2ConfigType;
}

Q_DECLARE_METATYPE(DataTypes::BitStringStruct)
Q_DECLARE_METATYPE(DataTypes::FloatWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::FloatStruct)
Q_DECLARE_METATYPE(DataTypes::SinglePointWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::FileStruct)
Q_DECLARE_METATYPE(DataTypes::FilesEnum)
Q_DECLARE_METATYPE(DataTypes::ConfParameterStruct)
Q_DECLARE_METATYPE(DataTypes::BlockStruct)
Q_DECLARE_METATYPE(DataTypes::ConfParametersListStruct)
Q_DECLARE_METATYPE(DataTypes::SignalsStruct)
Q_DECLARE_METATYPE(DataTypes::Signal)
Q_DECLARE_METATYPE(DataTypes::GeneralResponseStruct)
Q_DECLARE_METATYPE(DataTypes::DataRecV)
Q_DECLARE_METATYPE(Queries::Command)
#endif // DATATYPES_H
