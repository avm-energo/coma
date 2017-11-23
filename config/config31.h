#ifndef CONFIG31_H
#define CONFIG31_H

#define DIN31_NUMCH 11

#define DIN31_DEF_DLY       2
#define DIN31_DEF_PAIR      0xFFFF
#define DIN31_DEF_INTYPE    0

#include "config3x.h"
#include "../gen/publicclass.h"

class Config31
{
public:
    Config31(QVector<publicclass::DataRec> &config, bool BaseBoard=true);

    struct Bci
    {
        Config3x::Bci_DIn inblk;    // Блок с определениями входов
        Config3x::Bci_DOut outblk;  // Блок с определениями выходов
    };

    Bci Bci_block;

    void SetDefConf();
};

#endif // CONFIG31_H
