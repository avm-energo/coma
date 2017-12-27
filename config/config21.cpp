#include "config21.h"

Config21::Config21(QVector<publicclass::DataRec> &config, bool BaseBoard)
{
    // параметры входных сигналов
    quint32 StartInIndex = (BaseBoard) ? AIN21_MTB_STARTIDX : AIN21_MTM_STARTIDX;
    for (int i=0; i<config.size(); ++i)
    {
        if (config.at(i).id == 0xFFFFFFFF)
        {
            config.remove(i);
            --i;
        }
    }
    config.append({StartInIndex++, AIN21_NUMCH * sizeof(qint8), Bci_block.in_type});
    config.append({StartInIndex++, AIN21_NUMCH * sizeof(float), Bci_block.in_min});
    config.append({StartInIndex++, AIN21_NUMCH * sizeof(float), Bci_block.in_max});
    config.append({StartInIndex++, AIN21_NUMCH * sizeof(float), Bci_block.in_vmin});
    config.append({StartInIndex++, AIN21_NUMCH * sizeof(float), Bci_block.in_vmax});
    config.append({StartInIndex++, AIN21_NUMCH * sizeof(float), Bci_block.setminmin});
    config.append({StartInIndex++, AIN21_NUMCH * sizeof(float), Bci_block.setmin});
    config.append({StartInIndex++, AIN21_NUMCH * sizeof(float), Bci_block.setmax});
    config.append({StartInIndex++, AIN21_NUMCH * sizeof(float), Bci_block.setmaxmax});
    config.append({StartInIndex++, AIN21_NUMCH * sizeof(qint8), Bci_block.osc});
    config.append({StartInIndex++, sizeof(Bci_block.oscdly), &Bci_block.oscdly});
    config.append({StartInIndex++, sizeof(Bci_block.oscthr), &Bci_block.oscthr});
    config.append({StartInIndex++, sizeof(Bci_block.filterthr), &Bci_block.filterthr});
    config.append({StartInIndex++, sizeof(Bci_block.filtern), &Bci_block.filtern});
    config.append({StartInIndex++, sizeof(Bci_block.hysteresis), &Bci_block.hysteresis});
    config.append({0xFFFFFFFF, 0, NULL});
}

void Config21::SetDefConf()
{
    Bci_block.oscdly = AIN21_DEF_OSCDLY;
    Bci_block.oscthr = AIN21_DEF_OSCTHR;
    Bci_block.filtern = AIN21_DEF_FILTERN;
    Bci_block.filterthr = AIN21_DEF_FILTERTHR;
    Bci_block.hysteresis = AIN21_DEF_HYSTERS;
    for (int i = 0; i < AIN21_NUMCH; i++)
    {
        Bci_block.in_type[i] = AIN21_AIT_MA;
        Bci_block.in_min[i] = AIN21_DEF_INMIN;
        Bci_block.in_max[i] = AIN21_DEF_INMAX;
        Bci_block.in_vmin[i] = AIN21_DEF_INVMIN;
        Bci_block.in_vmax[i] = AIN21_DEF_INVMAX;
        Bci_block.setminmin[i] = AIN21_DEF_SETMINMIN;
        Bci_block.setmin[i] = AIN21_DEF_SETMIN;
        Bci_block.setmax[i] = AIN21_DEF_SETMAX;
        Bci_block.setmaxmax[i] = AIN21_DEF_SETMAXMAX;
        Bci_block.osc[i] = AIN21_DEF_OSC;
    }
}
