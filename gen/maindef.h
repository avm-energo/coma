#ifndef MAINDEF_H
#define MAINDEF_H

#define DEVICETYPE_MODULE 1 // модуль
#define DEVICETYPE_DEVICE 2 // прибор

// program sizes
#define PROGSIZE_EMUL   0
#define PROGSIZE_SMALL  1
#define PROGSIZE_MEDIUM 2
#define PROGSIZE_LARGE  3
#define PROGSIZE_FULL   4

// thresholds
#define TMAX    FLT_MAX
#define T25     25.0
#define T5      5.0
#define TH1     1.0
#define TH05    0.5
#define TH01    0.1
#define TH005   0.05
#define TH002   0.02
#define TH0005  0.005

// signals
#define S0      0.0
#define S1      1.0
#define S4      4.0
#define S20     20.0

#endif // MAINDEF_H
