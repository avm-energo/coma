#include "configa1.h"

ConfigA1::ConfigA1(QVector<publicclass::DataRec> &config)
{
    // параметры входных сигналов
    quint32 StartInIndex = IDA100_START;
    publicclass::DataRec configelement;
    bool FFound = false;
    foreach (configelement, config)
    {
        if (configelement.id == 0xFFFFFFFF)
        {
            FFound = true;
            break;
        }
    }
    if (!FFound)
        config.append({0xFFFFFFFF, 0, NULL});
    if (StartInIndex != 0)
    {
        config.append({StartInIndex, sizeof(Bci_block.DTCanal), &Bci_block.DTCanal});
        config.append({StartInIndex+1, sizeof(Bci_block.DHCanal), &Bci_block.DHCanal});
        config.append({StartInIndex+2, sizeof(Bci_block.DNFNum), &Bci_block.DNFNum});
        config.append({StartInIndex+3, sizeof(Bci_block.Filter), &Bci_block.Filter});
        config.append({StartInIndex+4, sizeof(Bci_block.Unom), &Bci_block.Unom});
        config.append({StartInIndex+5, sizeof(Bci_block.W100), &Bci_block.W100});
        config.append({StartInIndex+6, sizeof(Bci_block.T4), &Bci_block.T4});
        config.append({StartInIndex+7, sizeof(Bci_block.T20), &Bci_block.T20});
        config.append({StartInIndex+8, sizeof(Bci_block.H4), &Bci_block.H4});
        config.append({StartInIndex+9, sizeof(Bci_block.H20), &Bci_block.H20});
        config.append({StartInIndex+10, sizeof(Bci_block.DUosc), &Bci_block.DUosc});
        config.append({StartInIndex+11, sizeof(Bci_block.DUmin), &Bci_block.DUmin});
    }
}

void ConfigA1::SetDefConf()
{
    Bci_block.DTCanal = 1;
    Bci_block.DHCanal = 0;
    Bci_block.DNFNum = 1;
    Bci_block.Filter = 100;
    Bci_block.Unom = 330;
    Bci_block.W100 = 1.385;
    Bci_block.T4 = 0;
    Bci_block.T20 = 0;
    Bci_block.H4 = 0;
    Bci_block.H20 = 0;
    Bci_block.DUosc = 120;
    Bci_block.DUmin = 5;
}
