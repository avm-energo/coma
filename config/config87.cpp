#include "config87.h"

Config87::Config87(QVector<S2::DataRec> &config)
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

    config.append({StartInIndex++, CHR87_NUMCH * sizeof(qint8), &Bci_block.percentLimit});
    config.append({StartInIndex++, CHR87_NUMCH * sizeof(qint16), &Bci_block.T_down});
    config.append({StartInIndex++, CHR87_NUMCH * sizeof(qint16), &Bci_block.T_up});
    config.append({StartInIndex++, CHR87_NUMCH * sizeof(float), &Bci_block.F_down});
    config.append({StartInIndex++, CHR87_NUMCH * sizeof(float), &Bci_block.F_up});
    config.append({StartInIndex++, CHR87_NUMCH * sizeof(float), &Bci_block.A_down});
    config.append({StartInIndex++, CHR87_NUMCH * sizeof(float), &Bci_block.A_up});

    config.append({0xFFFFFFFF, 0, nullptr});

}

void Config87::SetDefConf()
{
    for (int i = 0; i < CHR87_NUMCH; i++)
    {
        Bci_block.percentLimit[i] = CHR87_DEF_PERCENTLIMIT;
        Bci_block.T_down[i] = CHR87_DEF_T_DOWN;
        Bci_block.T_up[i] = CHR87_DEF_T_UP;
        Bci_block.F_down[i] = CHR87_DEF_F_DOWN;
        Bci_block.F_up[i] = CHR87_DEF_F_UP;
        Bci_block.A_down[i] = CHR87_DEF_A_DOWN;
        Bci_block.A_up[i] = CHR87_DEF_A_UP;
    }
}
