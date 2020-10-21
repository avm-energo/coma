#ifndef DATATYPES_H
#define DATATYPES_H

#include <qmetatype.h>

namespace DataTypes
{
struct BitString
{
    //    quint8 sigAdr[3];
    quint32 sigAdr;
    quint32 sigVal;
    quint8 sigQuality;
}; // первое - номера сигналов, второе - их значения ("" ~ недостоверное
   // значение), третье - метка времени

struct FloatWithTime
{
    quint32 sigAdr;
    float sigVal;
    quint8 sigQuality;
    quint64 CP56Time;
};

struct Float
{
    quint32 sigAdr;
    float sigVal;
};

struct SinglePointWithTime
{
    quint32 sigAdr;
    quint8 sigVal;
    quint64 CP56Time;
};

struct File
{
    quint32 filenum;
    QByteArray filedata;
};

struct ConfParameter
{
    quint32 ID;
    QByteArray data;
};
}

namespace Queries
{
enum Commands104
{
    CM104_INTERROGATETIMEGR15,
    CM104_COM51WRITETIME,
    CM104_COM45,
    CM104_COM50,
    CM104_CORREADREQUEST,
    CM104_SELECTFILE,
    CM104_FILEREADY
};
enum CommandsMBS
{
    MBS_READHOLDINGREGISTERS = 0x03,
    MBS_READINPUTREGISTER = 0x04,
    MBS_WRITEMULTIPLEREGISTERS = 0x10
};

struct Command104
{
    Commands104 cmd;
    quint32 uintarg;
    float flarg;
    QByteArray ba;
};
struct CommandMBS
{
    CommandsMBS command;
    quint16 address;
    quint16 quantity;
    quint8 sizeBytes;
    QByteArray data;
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
}

Q_DECLARE_METATYPE(DataTypes::BitString)
Q_DECLARE_METATYPE(DataTypes::FloatWithTime)
Q_DECLARE_METATYPE(DataTypes::SinglePointWithTime)
Q_DECLARE_METATYPE(DataTypes::File)
Q_DECLARE_METATYPE(DataTypes::ConfParameter)
Q_DECLARE_METATYPE(Queries::Command104)
Q_DECLARE_METATYPE(Queries::CommandMBS)

#endif // DATATYPES_H
