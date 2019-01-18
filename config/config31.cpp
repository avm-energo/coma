#include "Config31.h"

Config31::Config31(QVector<S2::DataRec> &config, bool BaseBoard)
{
    int Type = (BaseBoard == BoardTypes::BT_BASE) ? Config::MTB_31 : Config::MTM_31;
//    Config3x::SetDInSize(Bci_block.inblk, Type);
    // параметры входных сигналов
    quint32 StartInIndex = Config3x::ModTypeMap().value(Type).DInStart;
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
        config.append({StartInIndex, sizeof(Bci_block.inblk.in_type), &(Bci_block.inblk.in_type)});
        config.append({StartInIndex+1, sizeof(Bci_block.inblk.dly_time), &(Bci_block.inblk.dly_time)});
        config.append({StartInIndex+2, sizeof(Bci_block.inblk.pair), &(Bci_block.inblk.pair)});
        config.append({StartInIndex+3, sizeof(Bci_block.inblk.SignalType), &(Bci_block.inblk.SignalType)});
    }
    config.append({0xFFFFFFFF, 0, nullptr});
}

void Config31::SetDefConf()
{
    for (int i = 0; i < DIN31_NUMCH; i++)
    {
        Bci_block.inblk.dly_time[i] = DIN31_DEF_DLY;
        Bci_block.inblk.in_type[i] = D_INTYPE_NORM;
        Bci_block.inblk.pair[i] = DIN31_DEF_PAIR;
        Bci_block.inblk.SignalType = DIN31_DEF_SIGNAL_TYPE;
    }
}
