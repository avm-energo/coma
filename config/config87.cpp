#include "config87.h"

Config87::Config87(QVector<S2::DataRec> &config, int BaseBoard)
{
    // параметры входных сигналов
    quint32 StartInIndex = CHR87_MTB_STARTIDX;
    for (int i=0; i<config.size(); ++i)
    {
        if (config.at(i).id == 0xFFFFFFFF)
        {
            config.remove(i);
            --i;
        }
    }

    config.append({StartInIndex++, sizeof(qint8), &Bci_block.percentLimit});
    config.append({StartInIndex++, sizeof(qint16), &Bci_block.T_down});
    config.append({StartInIndex++, sizeof(qint16), &Bci_block.T_up});
    config.append({StartInIndex++, sizeof(float), &Bci_block.F_down});
    config.append({StartInIndex++, sizeof(float), &Bci_block.F_up});
    config.append({StartInIndex++, sizeof(float), &Bci_block.A_down});
    config.append({StartInIndex++, sizeof(float), &Bci_block.A_up});

}

void Config87::SetDefConf()
{
    Bci_block.percentLimit = CHR87_DEF_PERCENTLIMIT;
    Bci_block.T_down = CHR87_DEF_T_DOWN;
    Bci_block.T_up = CHR87_DEF_T_UP;
    Bci_block.F_down = CHR87_DEF_F_DOWN;
    Bci_block.F_up = CHR87_DEF_F_UP;
    Bci_block.A_down = CHR87_DEF_A_DOWN;
    Bci_block.A_up = CHR87_DEF_A_UP;
}
