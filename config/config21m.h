#ifndef CONFIG21M_H
#define CONFIG21M_H

#define MOD_NUMINPUTS21     8

#include "config2x.h"

class Config21m
{
public:
    Config21m(QVector<publicclass::DataRec> &config);

    struct Bci
    {
        Config2x::Bci_AIn inblk;    // Блок с определениями входов
        Config2x::Bci_AOut outblk;  // Блок с определениями выходов
    };

    Bci Bci_block, Bci_defblock;
};

#endif // CONFIG21M_H
