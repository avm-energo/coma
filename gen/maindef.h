#ifndef MAINDEF_H
#define MAINDEF_H

#define DEVICETYPE_MODULE 1 // модуль
#define DEVICETYPE_DEVICE 2 // прибор

// interface types
#define I_UNKNOWN -1
#define I_USB 0
#define I_ETHERNET 1
#define I_RS485 2

#include <QtGlobal>

enum BoardTypes
{
    BT_NONE = 0,
    BT_BASE = 1,
    BT_MEZONIN = 2,
    BT_BSMZ = 3,
    BT_MODULE = 4
};

enum ConnectionStates
{
    ConnectedState,
    ClosingState
};

extern int MainInterface;
extern quint32 MTypeB;
extern quint32 MTypeM;
// extern int TheEnd;
// extern int StopRead;

#endif // MAINDEF_H
