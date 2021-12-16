#ifndef DATATYPES_H
#define DATATYPES_H
#include "datarecv.h"
#include "std_ext.h"
#include "uint24.h"

#include <QDataStream>
#include <QVariant>
#ifdef __linux__
#include <ctime>
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

    FileStruct(const FilesEnum num, const QByteArray &file) : ID(num), data(file)
    {
    }
    FileStruct(const quint8 num, const QByteArray &file) : ID(num), data(file)
    {
    }
    FileStruct(const quint8 num) : ID(num)
    {
    }
    std::underlying_type_t<FilesEnum> ID;
    QByteArray data;

    S2DataTypes::DataRec serialize()
    {
        return { { ID, quint32(data.size()) }, static_cast<void *>(data.data()) };
    }
};

inline QDataStream &operator<<(QDataStream &stream, const FileStruct &str)
{
#if QT_VERSION >= 0x050C00
    stream << str.ID;
#else
    stream << std::underlying_type_t<FilesEnum>(str.ID);
#endif
    stream << str.data;
    return stream;
}
inline QDataStream &operator>>(QDataStream &stream, FileStruct &str)
{
#if QT_VERSION >= 0x050C00
    stream >> str.ID;
#else
    stream >> *reinterpret_cast<std::underlying_type_t<FilesEnum> *>(&str.ID);
#endif

    stream >> str.data;
    return stream;
}

struct BlockStruct
{
    quint32 ID;
    QByteArray data;
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

struct GeneralResponseStruct
{
    GeneralResponseTypes type;
    quint64 data;
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
struct OscInfo
{
    // заголовок записи
    DataRecHeader typeHeader;
    quint32 id; // Тип файла - осциллограмма и количество осциллограмм в файле (10000, 10001 ...) <- неверное описание
    /// Время начала записи осциллограммы
    quint64 unixtime;
    /// ID первой осциллограммы в файле (определяет структуру точки и номер канала)
    quint32 idOsc0;
};
#pragma pack(pop)

#pragma pack(push) /* push current alignment to stack */
#pragma pack(1)    /* set alignment to 1 byte boundary */
struct SwitchJourInfo
{
    uint16_t fileNum;    // Номер файла
    uint32_t fileLength; // Размер файла
    uint32_t num;        // Порядковый номер переключения
    uint16_t numA;       // Порядковый номер аппарата
    uint8_t typeA;       // Тип аппарата
    uint8_t options; // Направление переключения, тип коммутации и коммутируемые фазы
    uint64_t time; // Время, когда произведено переключение
};

struct SwitchJourRecord
{
    uint32_t num;   // Порядковый номер переключения
    uint16_t numA;  // Порядковый номер аппарата
    uint8_t typeA;  // Тип аппарата
    uint8_t result; // Результат операции: успешно / с неисправностью
    uint64_t time;  // Время, когда произведено переключение
    uint8_t options; // Направление переключения, тип коммутации и коммутируемые фазы
    float amperage[3];    // Значение тока в момент выдачи команды
    float voltage[3];     // Значение напряжения в момент выдачи команды
    uint16_t ownTime[3];  // Собственное время коммутации
    uint16_t fullTime[3]; // Полное время коммутации (только для отключения, для включения будут нули)
    uint16_t movTime[3];  // Время перемещения главного контакта
    uint16_t archTime[3]; // Время горения дуги
    float idleTime[3];    // Время безоперационного простоя
    int16_t inaccuracy[3]; // Погрешность синхронной коммутации (только для соответствующего типа коммутации, для
                           // остальных типов нули
    float supplyVoltage;   // Напряжение питания цепей соленоидов
    float tOutside;        // Температура окружающей среды
    float tInside[3];      // Температура внутри привода
    float phyd[3];         // Давление в гидросистеме привода
    uint64_t oscTime;      // Время старта осциллограммы
    uint8_t Reserve;       // Резерв
    uint32_t timeF;        // Время записи в журнал
};

#pragma pack(pop) /* restore original alignment from stack */

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

Q_DECLARE_METATYPE(DataTypes::SingleCommand)
Q_DECLARE_METATYPE(uint24)
Q_DECLARE_METATYPE(DataTypes::BitStringStruct)
Q_DECLARE_METATYPE(DataTypes::FloatWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::FloatStruct)
Q_DECLARE_METATYPE(DataTypes::SinglePointWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::FileStruct)
Q_DECLARE_METATYPE(DataTypes::FilesEnum)
Q_DECLARE_METATYPE(DataTypes::BlockStruct)
Q_DECLARE_METATYPE(DataTypes::S2FilePack)
Q_DECLARE_METATYPE(DataTypes::SignalsStruct)
Q_DECLARE_METATYPE(DataTypes::Signal)
Q_DECLARE_METATYPE(DataTypes::GeneralResponseStruct)
Q_DECLARE_METATYPE(DataTypes::DataRecV)
Q_DECLARE_METATYPE(S2DataTypes::OscInfo)
Q_DECLARE_METATYPE(Queries::Command)
Q_DECLARE_METATYPE(S2DataTypes::SwitchJourInfo)

#endif // DATATYPES_H
