#ifndef CONFIG35_H
#define CONFIG35_H

#define AIN35_DEF_WDTYPE    0
#define AIN35_DEF_WDNOUT    2
#define AIN35_DEF_WDTM      0

#include "config3x.h"
#include "../publicclass.h"

class Config35
{
public:
    Config35(QVector<publicclass::DataRec> &config, bool BaseBoard=true);

    struct Bci
    {
        Config3x::Bci_DIn inblk;    // Блок с определениями входов
        Config3x::Bci_DOut outblk;  // Блок с определениями выходов
    };

    Bci Bci_block;

    void SetDefConf();
};

#endif // CONFIG31_H
