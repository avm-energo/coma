#ifndef DATATYPES_H
#define DATATYPES_H
#include "datarecv.h"

#include <QDataStream>
#include <QVariant>
#ifdef __linux__
#include <time.h>
Q_DECLARE_METATYPE(timespec);
#endif
namespace DataTypes
{

struct RecordPair
{
    DataRecV record;
    bool visibility = true;
};

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
    /// oscillogramm id min
    FileOscMin = 1000,
    /// oscillogram id max
    FileOscMax = 2999

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
#if QT_VERSION >= 0x051200
    stream << str.filenum;
#else
    stream << std::underlying_type_t<FilesEnum>(str.filenum);
#endif
    stream << str.filedata;
    return stream;
}
inline QDataStream &operator>>(QDataStream &stream, FileStruct &str)
{
#if QT_VERSION >= 0x051200
    stream >> str.filenum;
#else
    stream >> *reinterpret_cast<std::underlying_type_t<FilesEnum> *>(&str.filenum);
#endif

    stream >> str.filedata;
    return stream;
}

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

typedef BlockStruct HardwareStruct;
typedef QList<ConfParameterStruct> ConfParametersListStruct;

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
    quint64 flags;
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

#pragma pack(push) /* push current alignment to stack */
#pragma pack(1)    /* set alignment to 1 byte boundary */
struct OscInfo
{
    quint32 fileNum;    // номер файла осциллограмм
    quint32 fileLength; // длина файла за исключением FileHeader (16 байт)
    quint32 id; // Тип файла - осциллограмма и количество осциллограмм в файле (10000, 10001 ...)
    quint64 unixtime; // Время начала записи осциллограммы
    quint32 id0; // ID первой осциллограммы в файле (определяет структуру точки и номер канала)
};
#pragma pack(pop)
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
    QUSB_WriteBlkData,
    QUSB_ReqBlkData,
    QUSB_ReqBlkDataA,
    QUSB_ReqBlkDataTech,
    QUSB_WriteBlkDataTech,
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

}

namespace S2DataTypes
{
constexpr quint32 dummyElement = 0xffffffff;
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
    quint32 numByte;
};

/// Тип группы плат
struct DataRecT
{
    // заголовок записи
    DataRecHeader typeHeader;
    quint8 typeTheData[4];
    quint8 versionSoftware[4];
};
/// Файл ВПО в формате BIN
struct DataRecF
{
    /// заголовок записи
    DataRecHeader fileDataHeader;
    QByteArray data;
};

struct FileStruct
{
    FileHeader fileHeader;
    DataRecT type;
    DataRecF file;
    // заголовок пустой записи
    DataRecHeader void_recHeader;
};
typedef QVector<S2DataTypes::DataRec> S2ConfigType;

#pragma pack(push) /* push current alignment to stack */
#pragma pack(1)    /* set alignment to 1 byte boundary */
struct SwitchJourInfo
{
    quint16 fileNum;    // Номер файла
    quint32 fileLength; // Размер файла
    quint32 num;        // Порядковый номер переключения
    quint8 numA;        // Порядковый номер аппарата
    quint8 typeA;       // Тип аппарата
    quint64 time;       // Время, когда произведено переключение
    quint32 options; // Направление переключения, тип коммутации и коммутируемые фазы
};

#pragma pack(pop) /* restore original alignment from stack */

struct SwitchJourRecord
{
    quint32 num;    // Порядковый номер переключения
    quint8 numA;    // Порядковый номер аппарата
    quint8 typeA;   // Тип аппарата
    quint16 result; // Результат операции: успешно / с неисправностью
    quint64 time;   // Время, когда произведено переключение
    quint32 options; // Направление переключения, тип коммутации и коммутируемые фазы
    float amperage[3];   // Значение тока в момент выдачи команды
    float voltage[3];    // Значение напряжения в момент выдачи команды
    quint16 ownTime[3];  // Собственное время коммутации
    quint16 fullTime[3]; // Полное время коммутации (только для отключения, для включения будут нули)
    quint16 movTime[3];  // Время перемещения главного контакта
    quint16 archTime[3]; // Время горения дуги
    quint16 idleTime[3]; // Время безоперационного простоя
    quint16 inaccuracy[3]; // Погрешность синхронной коммутации (только для соответствующего типа коммутации, для
                           // остальных типов нули
    float supplyVoltage;   // Напряжение питания цепей соленоидов
    float tOutside;        // Температура окружающей среды
    float tInside[3];      // Температура внутри привода
    float phyd[3];         // Давление в гидросистеме привода
    quint64 oscTime;       // Время старта осциллограммы
    quint8 Reserve[4];     // Резерв
    quint32 timeF;         // Время записи в журнал
};

struct DataRecSwitchJour
{
    DataRecHeader header;
    SwitchJourRecord swjRecord;
};

struct OscHeader
{
    quint64 time; // время первой точки в Unix-формате
    float step;   // шаг по времени в мс
    quint32 len;  // длина осциллограммы в количестве точек по времени
};

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
Q_DECLARE_METATYPE(DataTypes::OscInfo)
Q_DECLARE_METATYPE(Queries::Command)

#endif // DATATYPES_H
