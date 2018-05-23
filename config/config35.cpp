#include "config35.h"

Config35::Config35(QVector<S2::DataRec> &config, bool BaseBoard)
{
    int Type = (BaseBoard) ? MTB_35 : MTM_35;
    // параметры входных сигналов
    quint32 StartInIndex = Config3x::ModTypeMap().value(Type).DOutStart;
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
        config.append({StartInIndex, sizeof(Bci_block.wd_type), &(Bci_block.wd_type)});
        config.append({StartInIndex+1, sizeof(Bci_block.wd_nout), &(Bci_block.wd_nout)});
        config.append({StartInIndex+2, sizeof(Bci_block.wd_tm), &(Bci_block.wd_tm)});
        config.append({StartInIndex+3, sizeof(Bci_block.pulse_short), &(Bci_block.pulse_short)});
        config.append({StartInIndex+4, sizeof(Bci_block.pulse_long), &(Bci_block.pulse_long)});
        config.append({StartInIndex+5, sizeof(Bci_block.out_type), &(Bci_block.out_type)});
    }
    config.append({0xFFFFFFFF, 0, NULL});
}

void Config35::SetDefConf()
{
    Bci_block.wd_type = D35_DEF_WDTYPE;
    Bci_block.wd_nout = D35_DEF_WDNOUT;
    Bci_block.wd_tm = D35_DEF_WDTM;
    Bci_block.pulse_short = D35_DEF_PULSH;
    Bci_block.pulse_long = D35_DEF_PULLN;
    for (int i=0; i<D35_CHNUM; ++i)
    {
        Bci_block.out_type[i] = D35_DEF_OUTTP;
    }
}
