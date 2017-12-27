#include "config22.h"

Config22::Config22(QVector<publicclass::DataRec> &config, bool BaseBoard)
{
    // параметры входных сигналов
    quint32 StartInIndex = (BaseBoard) ? AIN22_MTB_STARTIDX : AIN22_MTM_STARTIDX;
    for (int i=0; i<config.size(); ++i)
    {
        if (config.at(i).id == 0xFFFFFFFF)
        {
            config.remove(i);
            --i;
        }
    }
    config.append({StartInIndex++, AIN22_NUMCH * sizeof(float), Bci_block.W100});
    config.append({StartInIndex++, AIN22_NUMCH * sizeof(float), Bci_block.in_minT});
    config.append({StartInIndex++, AIN22_NUMCH * sizeof(float), Bci_block.in_maxT});
    config.append({StartInIndex++, AIN22_NUMCH * sizeof(float), Bci_block.setminminT});
    config.append({StartInIndex++, AIN22_NUMCH * sizeof(float), Bci_block.setminT});
    config.append({StartInIndex++, AIN22_NUMCH * sizeof(float), Bci_block.setmaxT});
    config.append({StartInIndex++, AIN22_NUMCH * sizeof(float), Bci_block.setmaxmaxT});
    config.append({StartInIndex++, sizeof(Bci_block.kf_filtrT), &Bci_block.kf_filtrT});
    config.append({StartInIndex++, sizeof(Bci_block.cnt_filtrT), &Bci_block.cnt_filtrT});
    config.append({StartInIndex++, sizeof(Bci_block.hysteresisT), &Bci_block.hysteresisT});
    config.append({StartInIndex++, AIN22_NUMCH * sizeof(quint32), &Bci_block.RzeroT});
    config.append({0xFFFFFFFF, 0, NULL});
}

void Config22::SetDefConf()
{
    Bci_block.cnt_filtrT = AIN22_DEF_CNTFILTRT;
    Bci_block.kf_filtrT = AIN22_DEF_KFFILTRT;
    Bci_block.hysteresisT = AIN22_DEF_HYSTERS;
    for (int i = 0; i < AIN22_NUMCH; i++)
    {
        Bci_block.W100[i] = AIN22_DEF_W100;
        Bci_block.in_minT[i] = AIN22_DEF_INMIN;
        Bci_block.in_maxT[i] = AIN22_DEF_INMAX;
        Bci_block.setminminT[i] = AIN22_DEF_SETMINMIN;
        Bci_block.setminT[i] = AIN22_DEF_SETMIN;
        Bci_block.setmaxT[i] = AIN22_DEF_SETMAX;
        Bci_block.setmaxmaxT[i] = AIN22_DEF_SETMAXMAX;
        Bci_block.RzeroT[i] = AIN22_DEF_RZEROT;
    }
}
