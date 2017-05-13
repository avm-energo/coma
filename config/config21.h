#ifndef CONFIG21_H
#define CONFIG21_H

#define AIN21_NUMCH 8

#define AIN21_DEF_OSCDLY    0
#define AIN21_DEF_OSCTHR    0.05f
#define AIN21_DEF_FILTERN   10
#define AIN21_DEF_FILTERTHR 0.5
#define AIN21_DEF_HYSTERS   0.02f
#define AIN21_DEF_INMIN     4
#define AIN21_DEF_INMAX     20
#define AIN21_DEF_INVMIN    0
#define AIN21_DEF_INVMAX    1000
#define AIN21_DEF_SETMINMIN 10
#define AIN21_DEF_SETMIN    50
#define AIN21_DEF_SETMAX    950
#define AIN21_DEF_SETMAXMAX 990
#define AIN21_DEF_OSC       0

#include "config2x.h"

class Config21
{
public:
    Config21(QVector<publicclass::DataRec> &config, bool BaseBoard=true);

    struct Bci
    {
        Config2x::Bci_AIn inblk;    // Блок с определениями входов
        Config2x::Bci_AOut outblk;  // Блок с определениями выходов
    };

    Bci Bci_block;

    void SetDefConf();
};

#endif // CONFIG21_H
