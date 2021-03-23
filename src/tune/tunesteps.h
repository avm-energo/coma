#ifndef TUNESTEPS_H
#define TUNESTEPS_H

enum KIVTuneSteps
{
    KIVTS_CHECKING = 1,
    KIVTS_PT100 = 2,
    KIVTS_ADCU = 3,
    KIVTS_ADCI = 4,
    KIVTS_60TUNING = 5,
    KIVTS_20TUNING = 6
};

enum TuneSteps84
{
    TS84_CHECKING = 1,
    TS84_ADCU = 2,
    TS84_ADCI = 3,
    TS84_60TUNING = 4,
    TS84_20TUNING = 5
};

#endif // TUNESTEPS_H
