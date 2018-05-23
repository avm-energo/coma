#ifndef CONFIG35_H
#define CONFIG35_H

#define D35_CHNUM       4
#define D35_DEF_WDTYPE  0
#define D35_DEF_WDNOUT  2
#define D35_DEF_WDTM    0
#define D35_DEF_PULSH   100
#define D35_DEF_PULLN   500
#define D35_DEF_OUTTP   0

#include "config3x.h"

class Config35
{
public:
    Config35(QVector<S2::DataRec> &config, bool BaseBoard=true);

    Config3x::Bci_DOut Bci_block;  // Блок с определениями выходов

    void SetDefConf();
};

#endif // CONFIG31_H
