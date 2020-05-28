#ifndef MAINDEF_H
#define MAINDEF_H

#define DEVICETYPE_MODULE 1 // модуль
#define DEVICETYPE_DEVICE 2 // прибор

// interface types
#define I_UNKNOWN   -1
#define I_USB       0
#define I_ETHERNET  1
#define I_RS485     2

// program sizes
#define PROGSIZE_EMUL   0
#define PROGSIZE_SMALL  1
#define PROGSIZE_MEDIUM 2
#define PROGSIZE_LARGE  3
#define PROGSIZE_FULL   4

// thresholds
#define TMAX    FLT_MAX
#define T25     25.0f
#define T5      5.0f
#define TH1     1.0f
#define TH05    0.5f
#define TH01    0.1f
#define TH005   0.05f
#define TH002   0.02f
#define TH0005  0.005f

// signals
#define S0      0.0
#define S1      1.0
#define S4      4.0
#define S20     20.0

enum ConnectionStates
{
    ConnectedState,
    ClosingState
};
extern int MainInterface;

#endif // MAINDEF_H
