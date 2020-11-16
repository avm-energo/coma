#pragma once

#define POLLINGINTERVAL 300 // polling cycle time
#define SIGNALGROUPSNUM 7
#define MAINSLEEPCYCLETIME 50

#define READHOLDINGREGISTERS 0x03
#define READINPUTREGISTER 0x04
#define WRITEMULTIPLEREGISTERS 0x10

//#define INITREG 4000
//#define INITREGCOUNT 11 // 11 floats results in 22 bytes
#define TIMEREG 4600
#define TIMEREGCOUNT 1
#define SETINITREG 900
#define CLEARREG 905

namespace CommandsMBS
{
enum CommandRegisters
{
    SetStartupValuesReg = 900,
    ClearStartupValuesReg = 905
};

enum Commands
{
    MBS_READHOLDINGREGISTERS,
    MBS_READINPUTREGISTER,
    MBS_WRITEMULTIPLEREGISTERS,
    MBS_READCOILS
};

struct CommandStruct
{
    Commands cmd;
    quint16 adr;
    quint16 quantity;
    //    quint8 sizebytes;
    QByteArray data;
};

enum ModbusGroupsEnum
{
    SIGNALTYPE = 0,
    FIRSTBYTEADR = 1,
    SECONDBYTEADR = 2,
    FIRSTBYTEQ = 3,
    SECONDBYTEQ = 4
};

}

Q_DECLARE_METATYPE(CommandsMBS::CommandStruct)
