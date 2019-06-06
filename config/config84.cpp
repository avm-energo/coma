#include "config84.h"

Config84::Config84(QVector<S2::DataRec> &config)
{
    // параметры входных сигналов
    quint32 StartInIndex = ID8084_START;
    quint32 StartComIndex = ID8084_COM;
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
        config.append({1003, sizeof(Bci_block.NFiltr), &Bci_block.NFiltr});
        config.append({1006, sizeof(Bci_block.Unom), &Bci_block.Unom});
        config.append({StartInIndex, sizeof(Bci_block.Imax), &Bci_block.Imax});
        config.append({StartInIndex+1, sizeof(Bci_block.Umin), &Bci_block.Umin});
        config.append({StartInIndex+2, sizeof(Bci_block.Imin), &Bci_block.Imin});
        config.append({StartInIndex+3, sizeof(Bci_block.C_init), &Bci_block.C_init});
        config.append({StartInIndex+4, sizeof(Bci_block.Tg_init), &Bci_block.Tg_init});
        config.append({StartInIndex+5, sizeof(Bci_block.corTg), &Bci_block.corTg});
        config.append({StartInIndex+6, sizeof(Bci_block.dC_pred), &Bci_block.dC_pred});
        config.append({StartInIndex+7, sizeof(Bci_block.dC_alarm), &Bci_block.dC_alarm});
        config.append({StartInIndex+8, sizeof(Bci_block.dTg_pred), &Bci_block.dTg_pred});
        config.append({StartInIndex+9, sizeof(Bci_block.dTg_alarm), &Bci_block.dTg_alarm});
        config.append({StartInIndex+10, sizeof(Bci_block.dIunb_pred), &Bci_block.dIunb_pred});
        config.append({StartInIndex+11, sizeof(Bci_block.dIunb_alarm), &Bci_block.dIunb_alarm});
        config.append({StartInIndex+12, sizeof(Bci_block.GdC), &Bci_block.GdC});
        config.append({StartInIndex+13, sizeof(Bci_block.GdTg), &Bci_block.GdTg});
        config.append({StartInIndex+14, sizeof(Bci_block.GdIunb), &Bci_block.GdIunb});
        config.append({StartInIndex+15, sizeof(Bci_block.RTerm), &Bci_block.RTerm});
        config.append({StartInIndex+16, sizeof(Bci_block.W100), &Bci_block.W100});
        config.append({StartInIndex+17, sizeof(Bci_block.Iunb_init), &Bci_block.Iunb_init});
        config.append({StartInIndex+18, sizeof(Bci_block.Phy_unb_init), &Bci_block.Phy_unb_init});
        config.append({StartInIndex+19, sizeof(Bci_block.T_Data_Rec), &Bci_block.T_Data_Rec});

        if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
        {
          config.append({StartComIndex, sizeof(Com_param.IP), &Com_param.IP});
          config.append({StartComIndex+1, sizeof(Com_param.Mask), &Com_param.Mask});
          config.append({StartComIndex+2, sizeof(Com_param.GateWay), &Com_param.GateWay});
          config.append({StartComIndex+3, sizeof(Com_param.Port), &Com_param.Port});
          config.append({StartComIndex+4, sizeof(Com_param.SNTP), &Com_param.SNTP});
          config.append({StartComIndex+5, sizeof(Com_param.baud), &Com_param.baud});
          config.append({StartComIndex+6, sizeof(Com_param.parity), &Com_param.parity});
          config.append({StartComIndex+7, sizeof(Com_param.stopbit), &Com_param.stopbit});
          config.append({StartComIndex+8, sizeof(Com_param.adrMB), &Com_param.adrMB});
          config.append({StartComIndex+9, sizeof(Com_param.isNTP), &Com_param.isNTP});
        }
    }
    config.append({0xFFFFFFFF, 0, nullptr});

}

void Config84::SetDefConf()
{
    Bci_block.Unom = 220;
    Bci_block.Umin = 0.5;
    Bci_block.Imin = 0.5;
    Bci_block.dC_pred = 0.5;
    Bci_block.dC_alarm = 1;
    Bci_block.dTg_pred = 1;
    Bci_block.dTg_alarm = 1.5;
    Bci_block.dIunb_pred = 1;
    Bci_block.dIunb_alarm = 1.5;
    Bci_block.GdC = 0.1f;
    Bci_block.GdTg = 0.2f;
    Bci_block.GdIunb = 0.2f;
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
    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    {
        Bci_block.RTerm = 100;
        Bci_block.W100 = 1.385f;

        Com_param.IP[0] = 172;
        Com_param.IP[1] = 16;
        Com_param.IP[2] = 28;
        Com_param.IP[3] = 5;

        Com_param.Mask[0] = 255;
        Com_param.Mask[1] = 255;
        Com_param.Mask[2] = 0;
        Com_param.Mask[3] = 0;

        Com_param.GateWay[0] = 172;
        Com_param.GateWay[1] = 16;
        Com_param.GateWay[2] = 28;
        Com_param.GateWay[3] = 1;

        Com_param.Port[0] = 2404;
        Com_param.Port[1] = 2405;
        Com_param.Port[2] = 502;
        Com_param.Port[3] = 502;

        Com_param.SNTP[0] = 172;
        Com_param.SNTP[1] = 16;
        Com_param.SNTP[2] = 31;
        Com_param.SNTP[3] = 6;

        Com_param.baud = 9600;
        Com_param.parity = 0;
        Com_param.stopbit = 0;
        Com_param.adrMB = 1;
        Com_param.isNTP = 1;

    }
}
