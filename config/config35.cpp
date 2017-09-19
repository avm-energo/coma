#include "config35.h"

Config35::Config35(QVector<publicclass::DataRec> &config, bool BaseBoard)
{
    int Type = (BaseBoard) ? MTB_31 : MTM_31;
    Config3x::SetDInSize(Bci_block.inblk, Type);
    // параметры входных сигналов
    quint32 StartInIndex = Config3x::ModTypeMap().value(Type).DInStart;
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
        config.append({StartInIndex, sizeof(Bci_block.inblk.in_type), Bci_block.inblk.in_type.data()});
        config.append({StartInIndex+1, sizeof(Bci_block.inblk.dly_time), Bci_block.inblk.dly_time.data()});
        config.append({StartInIndex+2, sizeof(Bci_block.inblk.pair), Bci_block.inblk.pair.data()});
    }
}

void Config35::SetDefConf()
{
    for (int i = 0; i < AIN31_NUMCH; i++)
    {
        Bci_block.inblk.dly_time[i] = AIN31_DEF_DLY;
        Bci_block.inblk.in_type[i] = Config3x::DIT_NORM;
        Bci_block.inblk.pair[i] = AIN31_DEF_PAIR;
    }
}
