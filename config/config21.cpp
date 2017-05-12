#include "config21.h"

Config21::Config21(QVector<publicclass::DataRec> &config, bool BaseBoard)
{
    int Type = (BaseBoard) ? MTB_21 : MTM_21;
    Config2x::SetAInSize(Bci_block.inblk, Type);
    // параметры входных сигналов
    quint32 StartInIndex = Config2x::ModTypeMap().value(Type).AInStart;
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
        config.append({StartInIndex+1, sizeof(Bci_block.inblk.in_min), Bci_block.inblk.in_min.data()});
        config.append({StartInIndex+2, sizeof(Bci_block.inblk.in_max), Bci_block.inblk.in_max.data()});
        config.append({StartInIndex+3, sizeof(Bci_block.inblk.in_vmin), Bci_block.inblk.in_vmin.data()});
        config.append({StartInIndex+4, sizeof(Bci_block.inblk.in_vmax), Bci_block.inblk.in_vmax.data()});
        config.append({StartInIndex+5, sizeof(Bci_block.inblk.setminmin), Bci_block.inblk.setminmin.data()});
        config.append({StartInIndex+6, sizeof(Bci_block.inblk.setmin), Bci_block.inblk.setmin.data()});
        config.append({StartInIndex+7, sizeof(Bci_block.inblk.setmax), Bci_block.inblk.setmax.data()});
        config.append({StartInIndex+8, sizeof(Bci_block.inblk.setmaxmax), Bci_block.inblk.setmaxmax.data()});
        config.append({StartInIndex+9, sizeof(Bci_block.inblk.osc), Bci_block.inblk.osc.data()});
        config.append({StartInIndex+10, sizeof(Bci_block.inblk.oscdly), &Bci_block.inblk.oscdly});
        config.append({StartInIndex+11, sizeof(Bci_block.inblk.oscthr), &Bci_block.inblk.oscthr});
        config.append({StartInIndex+12, sizeof(Bci_block.inblk.filterthr), &Bci_block.inblk.filterthr});
        config.append({StartInIndex+13, sizeof(Bci_block.inblk.filtern), &Bci_block.inblk.filtern});
        config.append({StartInIndex+14, sizeof(Bci_block.inblk.hysteresis), &Bci_block.inblk.hysteresis});
    }
}

void Config21::SetDefConf()
{
    Bci_defblock.inblk.oscdly = AIN21_DEF_OSCDLY;
    Bci_defblock.inblk.oscthr = AIN21_DEF_OSCTHR;
    Bci_defblock.inblk.filtern = AIN21_DEF_FILTERN;
    Bci_defblock.inblk.filterthr = AIN21_DEF_FILTERTHR;
    Bci_defblock.inblk.hysteresis = AIN21_DEF_HYSTERS;
    for (int i = 0; i < AIN21_NUMCH; i++)
    {
        Bci_defblock.inblk.in_type[i] = Config2x::AIT_MA;
        Bci_defblock.inblk.in_min[i] = AIN21_DEF_INMIN;
        Bci_defblock.inblk.in_max[i] = AIN21_DEF_INMAX;
        Bci_defblock.inblk.in_vmin[i] = AIN21_DEF_INVMIN;
        Bci_defblock.inblk.in_vmax[i] = AIN21_DEF_INVMAX;
        Bci_defblock.inblk.setminmin[i] = AIN21_DEF_SETMINMIN;
        Bci_defblock.inblk.setmin[i] = AIN21_DEF_SETMIN;
        Bci_defblock.inblk.setmax[i] = AIN21_DEF_SETMAX;
        Bci_defblock.inblk.setmaxmax[i] = AIN21_DEF_SETMAXMAX;
        Bci_defblock.inblk.osc[i] = AIN21_DEF_OSC;
    }
}
