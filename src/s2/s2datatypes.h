#ifndef S2DATATYPES_H
#define S2DATATYPES_H

#include "filestruct.h"

#include <QVariant>
#include <gen/std_ext.h>

namespace S2DataTypes
{
constexpr quint32 dummyElement = 0xffffffff;

inline bool is_same(const DataRec &lhs, const DataRec &rhs)
{
    bool is_same_value = false;
    if ((lhs.header.id == rhs.header.id) && (lhs.header.numByte == rhs.header.numByte))
        is_same_value = !memcmp(lhs.thedata, rhs.thedata, lhs.header.numByte);

    Q_ASSERT(is_same_value);
    return is_same_value;
}

#pragma pack(push) /* push current alignment to stack */
#pragma pack(1)    /* set alignment to 1 byte boundary */
struct OscInfo
{
    // заголовок записи
    DataRecHeader typeHeader;
    quint32 id; ///< Тип файла - осциллограмма и количество осциллограмм в файле (10000, 10001 ...) <- неверное описание
    quint64 unixtime; ///< Время начала записи осциллограммы
    quint32 idOsc0; ///< ID первой осциллограммы в файле (определяет структуру точки и номер канала)

    friend QDebug operator<<(QDebug debug, const S2DataTypes::OscInfo &st);
};

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

/// S2: Определение типа заголовка
/// Заголовок файла (Прил. 1)
struct S2FileHeader
{
    quint16 fname;
    quint16 service;
    quint32 size;
    quint32 crc32;
    quint32 thetime;
};

/// \brief S2B File header format.
struct S2BFileHeader
{
    quint16 fname;
    quint16 types2b;
    quint32 size;
    quint16 typeB;
    quint16 typeM;
    quint32 thetime;
};

/// \brief S2B File tail format.
struct S2BFileTail
{
    quint32 crc32;
    quint32 end;
};

/// \brief S2B Binary file format.
struct S2BFile
{
    S2BFileHeader header;
    DataTypes::FileStruct file;
    S2BFileTail tail;
};

/// Тип группы плат
struct DataRecT
{
    ///< заголовок записи
    DataRecHeader typeHeader;
    quint8 typeTheData[4];
    quint8 versionSoftware[4];
};

/// Файл ВПО в формате BIN
struct DataRecF
{
    ///< заголовок записи
    DataRecHeader fileDataHeader;
    QByteArray data;
};

struct FileStruct
{
    S2FileHeader fileHeader;
    DataRecT type;
    DataRecF file;
    // заголовок пустой записи
    DataRecHeader void_recHeader;
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

Q_DECLARE_METATYPE(S2DataTypes::S2BFile)
Q_DECLARE_METATYPE(S2DataTypes::OscInfo)
Q_DECLARE_METATYPE(S2DataTypes::SwitchJourInfo)

#endif // S2DATATYPES_H
