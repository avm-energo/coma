#ifndef MAINDEF_H
#define MAINDEF_H

#define DEVICETYPE_MODULE 1 // модуль
#define DEVICETYPE_DEVICE 2 // прибор

// interface types
#define I_UNKNOWN -1
#define I_USB 0
#define I_ETHERNET 1
#define I_RS485 2

// thresholds
#define TMAX FLT_MAX
#define T25 25.0f
#define T5 5.0f
#define TH1 1.0f
#define TH05 0.5f
#define TH01 0.1f
#define TH005 0.05f
#define TH002 0.02f
#define TH0005 0.005f

// signals
#define S0 0.0
#define S1 1.0
#define S4 4.0
#define S20 20.0

#include "qglobal.h"

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
