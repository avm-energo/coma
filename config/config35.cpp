#include "config35.h"

Config35::Config35(QVector<publicclass::DataRec> &config, bool BaseBoard)
{
    int Type = (BaseBoard) ? MTB_35 : MTM_35;
    Config3x::SetDOutSize(Bci_block.outblk, Type);
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
        config.append({StartInIndex, sizeof(Bci_block.outblk.wd_type), &(Bci_block.outblk.wd_type)});
        config.append({StartInIndex+1, sizeof(Bci_block.outblk.wd_nout), &(Bci_block.outblk.wd_nout)});
        config.append({StartInIndex+2, sizeof(Bci_block.outblk.wd_tm), &(Bci_block.outblk.wd_tm)});
    }
    config.append({0xFFFFFFFF, 0, NULL});
}

void Config35::SetDefConf()
{
    Bci_block.outblk.wd_type = AIN35_DEF_WDTYPE;
    Bci_block.outblk.wd_nout = AIN35_DEF_WDNOUT;
    Bci_block.outblk.wd_tm = AIN35_DEF_WDTM;
}
