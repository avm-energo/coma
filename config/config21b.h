#ifndef CONFIG21B_H
#define CONFIG21B_H

#define MOD_NUMINPUTS21     8

#include "config2x.h"

class Config21b
{
public:
    Config21b(QVector<publicclass::DataRec> &config);

    struct Bci
    {
        Config2x::Bci_AIn inblk;    // Блок с определениями входов
        Config2x::Bci_AOut outblk;  // Блок с определениями выходов
    };

    Bci Bci_block, Bci_defblock;
};

#endif // CONFIG21B_H
