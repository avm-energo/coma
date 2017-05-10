#include "config21m.h"

Config21m::Config21m(QVector<publicclass::DataRec> &config)
{
    Config2x::SetAInSize(Bci_block.inblk, MTM_21);
    // параметры входных сигналов
    int StartInIndex = Config2x::ModTypeMap().value(MTM_21).AInStart;
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

    Bci_defblock.inblk.oscdly = 0;
    Bci_defblock.inblk.oscthr = 0.05f;
    Bci_defblock.inblk.filtern = 10;
    Bci_defblock.inblk.filterthr = 0.5;
    Bci_defblock.inblk.hysteresis = 0.02f;
    for (int i = 0; i < MOD_NUMINPUTS21; ++i)
    {
        Bci_defblock.inblk.in_type[i] = INTYPEMA;
        Bci_defblock.inblk.in_min[i] = 4;
        Bci_defblock.inblk.in_max[i] = 20;
        Bci_defblock.inblk.in_vmin[i] = 0;
        Bci_defblock.inblk.in_vmax[i] = 1000;
        Bci_defblock.inblk.setminmin[i] = 10;
        Bci_defblock.inblk.setmin[i] = 50;
        Bci_defblock.inblk.setmax[i] = 950;
        Bci_defblock.inblk.setmaxmax[i] = 990;
        Bci_defblock.inblk.osc[i] = 0;
    }
}
