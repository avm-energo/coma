#include "config80.h"

Config80::Config80(QVector<S2::DataRec> &config)
{
    // параметры входных сигналов
    quint32 StartInIndex = ID808X_START;
    for (int i=0; i<config.size(); ++i)
    {
        if (config.at(i).id == 0xFFFFFFFF)
        {
            config.remove(i);
            --i;
        }
    }
    if (StartInIndex != 0)
    {
        config.append({StartInIndex, sizeof(Bci_block.eq_type), &Bci_block.eq_type});
        config.append({StartInIndex+1, sizeof(Bci_block.npoints), &Bci_block.npoints});
        config.append({StartInIndex+2, sizeof(Bci_block.nfiltr), &Bci_block.nfiltr});
        config.append({StartInIndex+3, sizeof(Bci_block.nhfiltr), &Bci_block.nhfiltr});
        config.append({StartInIndex+4, sizeof(Bci_block.ddosc), &Bci_block.ddosc});
        config.append({StartInIndex+5, sizeof(Bci_block.unom1), &Bci_block.unom1});
        config.append({StartInIndex+6, sizeof(Bci_block.unom2), &Bci_block.unom2});
        config.append({StartInIndex+7, sizeof(Bci_block.inom1), Bci_block.inom1});
        config.append({StartInIndex+8, sizeof(Bci_block.inom2), Bci_block.inom2});
        config.append({StartInIndex+9, sizeof(Bci_block.duosc), &Bci_block.duosc});
        config.append({StartInIndex+10, sizeof(Bci_block.diosc), &Bci_block.diosc});
        config.append({StartInIndex+11, sizeof(Bci_block.duimin), &Bci_block.duimin});
    }
    config.append({0xFFFFFFFF, 0, NULL});
}

void Config80::SetDefConf()
{
    Bci_block.eq_type = 1;
    Bci_block.npoints = 128;
    Bci_block.nfiltr = 100;
    Bci_block.nhfiltr = 3;
    Bci_block.ddosc = 1;
    Bci_block.unom1 = 500;
    Bci_block.unom2 = 220;
    Bci_block.duosc = 2.0;
    Bci_block.diosc = 2.0;
    Bci_block.duimin = 0.5;
    for (int i = 0; i < 3; i++)
    {
        Bci_block.inom1[i] = 600;
        Bci_block.inom1[i+3] = 1000;
        Bci_block.inom2[i] = Bci_block.inom2[i+3] = 5;
    }
}
