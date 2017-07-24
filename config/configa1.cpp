#include "configa1.h"
#include "config80.h"

ConfigA1::ConfigA1(QVector<publicclass::DataRec> &config)
{
    // параметры входных сигналов
    quint32 StartInIndex = IDA100_START;
    quint32 StartInIndex8x = ID808X_START;
    if (StartInIndex != 0)
    {
        config.append({StartInIndex, sizeof(Bci_block.DTCanal), &Bci_block.DTCanal});
        config.append({StartInIndex+1, sizeof(Bci_block.DHCanal), &Bci_block.DHCanal});
        config.append({StartInIndex+2, sizeof(Bci_block.DNFNum), &Bci_block.DNFNum});
        config.append({StartInIndex+3, sizeof(Bci_block.K_DN), &Bci_block.K_DN});
        config.append({StartInIndex+4, sizeof(Bci_block.W100), &Bci_block.W100});
        config.append({StartInIndex+5, sizeof(Bci_block.T4), &Bci_block.T4});
        config.append({StartInIndex+6, sizeof(Bci_block.T20), &Bci_block.T20});
        config.append({StartInIndex+7, sizeof(Bci_block.H4), &Bci_block.H4});
        config.append({StartInIndex+8, sizeof(Bci_block.H20), &Bci_block.H20});
        config.append({StartInIndex8x+1, sizeof(Bci_block.NPoints), &Bci_block.NPoints});
        config.append({StartInIndex8x+2, sizeof(Bci_block.Filter), &Bci_block.Filter});
        config.append({StartInIndex8x+3, sizeof(Bci_block.NHarmFilt), &Bci_block.NHarmFilt});
        config.append({StartInIndex8x+4, sizeof(Bci_block.DDOsc), &Bci_block.DDOsc});
        config.append({StartInIndex8x+9, sizeof(Bci_block.DUosc), &Bci_block.DUosc});
        config.append({StartInIndex8x+11, sizeof(Bci_block.DUmin), &Bci_block.DUmin});
    }
    for (int i=0; i<config.size(); ++i)
    {
        if (config.at(i).id == 0xFFFFFFFF)
            config.removeAt(i);
    }
    config.append({0xFFFFFFFF, 0, NULL});
}

void ConfigA1::SetDefConf()
{
    Bci_block.DTCanal = 1;
    Bci_block.DHCanal = 0;
    Bci_block.DNFNum = 1;
    Bci_block.NPoints = 256;
    Bci_block.Filter = 100;
    Bci_block.NHarmFilt = 3;
    Bci_block.K_DN = 2200; // 500 кВ
    Bci_block.W100 = static_cast<float>(1.385);
    Bci_block.T4 = -50;
    Bci_block.T20 = 50;
    Bci_block.H4 = 0;
    Bci_block.H20 = 100;
    Bci_block.DDOsc = 0;
    Bci_block.DUosc = 10.0;
    Bci_block.DUmin = 1.0;
}
