#include "config84.h"

Config84::Config84(QVector<S2::DataRec> &config)
{
    // параметры входных сигналов
    quint32 StartInIndex = ID8084_START;
    for (int i=0; i<config.size(); ++i)
    {
        if (config.at(i).id == 0xFFFFFFFF)
        {
            config.remove(i);
            --i;
        }
    }

    if ((StartInIndex != 0))
    {
        config.append({StartInIndex, sizeof(Bci_block.Unom), &Bci_block.Unom});
        config.append({StartInIndex+1, sizeof(Bci_block.Imax), &Bci_block.Imax});
        config.append({StartInIndex+2, sizeof(Bci_block.Umin), &Bci_block.Umin});
        config.append({StartInIndex+3, sizeof(Bci_block.Imin), &Bci_block.Imin});
        config.append({StartInIndex+4, sizeof(Bci_block.C_init), &Bci_block.C_init});
        config.append({StartInIndex+5, sizeof(Bci_block.Tg_init), &Bci_block.Tg_init});
        config.append({StartInIndex+6, sizeof(Bci_block.corTg), &Bci_block.corTg});
        config.append({StartInIndex+7, sizeof(Bci_block.dС_pred), &Bci_block.dС_pred});
        config.append({StartInIndex+8, sizeof(Bci_block.dС_alarm), &Bci_block.dС_alarm});
        config.append({StartInIndex+9, sizeof(Bci_block.dTg_pred), &Bci_block.dTg_pred});
        config.append({StartInIndex+10, sizeof(Bci_block.dTg_alarm), &Bci_block.dTg_alarm});
        config.append({StartInIndex+11, sizeof(Bci_block.dIunb_pred), &Bci_block.dIunb_pred});
        config.append({StartInIndex+12, sizeof(Bci_block.dIunb_alarm), &Bci_block.dIunb_alarm});
        config.append({StartInIndex+13, sizeof(Bci_block.GdС), &Bci_block.GdС});
        config.append({StartInIndex+14, sizeof(Bci_block.GdTg), &Bci_block.GdTg});
        config.append({StartInIndex+15, sizeof(Bci_block.GdIunb), &Bci_block.GdIunb});
        config.append({StartInIndex+16, sizeof(Bci_block.RTerm), &Bci_block.RTerm});
        config.append({StartInIndex+17, sizeof(Bci_block.W100), &Bci_block.W100});
        config.append({StartInIndex+18, sizeof(Bci_block.Iunb_init), &Bci_block.Iunb_init});
        config.append({StartInIndex+19, sizeof(Bci_block.Phy_unb_init), &Bci_block.Phy_unb_init});
        config.append({StartInIndex+20, sizeof(Bci_block.NFiltr), &Bci_block.NFiltr});
        config.append({StartInIndex+21, sizeof(Bci_block.T_Data_Rec), &Bci_block.T_Data_Rec});
    }
    config.append({0xFFFFFFFF, 0, nullptr});

}

void Config84::SetDefConf()
{
    Bci_block.Unom = 220;
    Bci_block.Umin = 0.5;
    Bci_block.Imin = 0.5;
    Bci_block.dС_pred = 0.5;
    Bci_block.dС_alarm = 1;
    Bci_block.dTg_pred = 1;
    Bci_block.dTg_alarm = 1.5;
    Bci_block.dIunb_pred = 1;
    Bci_block.dIunb_alarm = 1.5;
    Bci_block.GdС = 0.1;
    Bci_block.GdTg = 0.2;
    Bci_block.GdIunb = 0.2;
    Bci_block.RTerm = 100;
    Bci_block.W100 = 1.385;
    Bci_block.Iunb_init = 0;
    Bci_block.Phy_unb_init = 0;
    Bci_block.NFiltr = 100;
    Bci_block.T_Data_Rec = 3600;

    for (int i = 0; i < 3; i++)
    {
        Bci_block.Imax[i] = 700;
        Bci_block.C_init[i] = 2200;
        Bci_block.Tg_init[i] = 0;
        Bci_block.corTg[i] = 0;
    }
}
