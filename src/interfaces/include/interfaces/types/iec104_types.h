#pragma once

#include <interfaces/types/common_types.h>
#include <interfaces/types/iec104/apci.h>
#include <interfaces/types/iec104/asdu.h>

constexpr quint32 BSIGROUP = 1;
constexpr quint32 STARTUPGROUP = 2;
constexpr quint32 ALARMGROUP = 3;
constexpr quint32 MAINFLOATGROUP = 4;
constexpr quint32 MAINBITSTRINGGROUP = 4;
constexpr quint32 TIMEGROUP = 15;

#define I104_START 0x68

#define I104_I 0x00
#define I104_S 0x01
#define I104_U 0x03
#define I104_WRONG 0xFF

#define I104_W 8

#define I104_STARTDT_ACT 0x07
#define I104_STARTDT_CON 0x0B
#define I104_STOPDT_ACT 0x13
#define I104_STOPDT_CON 0x23
#define I104_TESTFR_ACT 0x43
#define I104_TESTFR_CON 0x83u

// определения возвращаемого значения функции isIncomeDataValid
#define I104_RCVNORM 0x00
#define I104_RCVSMSIZE 0x01
#define I104_RCVWRONG 0x02

#define SECTIONSIZE 2048
#define SEGMENTSIZE 230
#define OFFSET 13
#define FHSIZE 16
#define SYSTEM_JOUR_SIZE 65568

namespace Iec104
{

enum Commands
{
    // New
    RequestGroup,
    RequestFile,
    RequestConfigFile,
    WriteFile,
    COM45,
    COM50,
    COM51,
    // Old
    CM104_REQGROUP,
    CM104_COM51,
    CM104_COM45,
    CM104_COM50,
    CM104_REQFILE,
    CM104_REQCONFIGFILE,
    CM104_WRITEFILE
};

struct CommandStruct
{
    Commands cmd;
    quint32 address;
    float flarg;
    bool blarg;
    QByteArray ba;
};

struct QualifierVariableStructute
{
    quint8 Number; // number of Informational Objects
    quint8 SQ;     // Single <0> or Series <1> of Objects
};

struct COT
{
    quint8 cause;     // <0..63> cause number
    quint8 confirm;   // <0> - positive , <1> - negative
    quint8 test;      // <0> - not a test, <1> - test
    quint8 initiator; // number of initiating address
};

struct DataUnitIdentifier
{
    MessageDataType typeIdent;
    QualifierVariableStructute qualifier;
    COT cause;
    quint8 commonAdrASDU;
};

}
Q_DECLARE_METATYPE(Iec104::CommandStruct)
