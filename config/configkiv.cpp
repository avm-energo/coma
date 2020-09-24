#include "configkiv.h"

ConfigKIV::ConfigKIV(QVector<S2::DataRec> *config)
{
    // параметры входных сигналов
    quint32 StartInIndex = ID8084_START;
    //    quint32 StartComIndex = ID8084_COM;
    for (int i = 0; i < config->size(); ++i)
    {
        if (config->at(i).id == 0xFFFFFFFF)
        {
            config->remove(i);
            --i;
        }
    }

    if (StartInIndex != 0)
    {

        config->append({ 1003, sizeof(Bci_block.NFiltr), &Bci_block.NFiltr });
        config->append({ 1006, sizeof(Bci_block.Unom), &Bci_block.Unom });
        config->append({ StartInIndex, sizeof(Bci_block.Umin), &Bci_block.Umin });
        config->append({ StartInIndex + 1, sizeof(Bci_block.Imin), &Bci_block.Imin });
        config->append({ StartInIndex + 3, sizeof(Bci_block.C_init), &Bci_block.C_init });
        config->append({ StartInIndex + 4, sizeof(Bci_block.Tg_init), &Bci_block.Tg_init });
        config->append({ StartInIndex + 5, sizeof(Bci_block.corTg), &Bci_block.corTg });
        config->append({ StartInIndex + 6, sizeof(Bci_block.dC_pred), &Bci_block.dC_pred });
        config->append({ StartInIndex + 7, sizeof(Bci_block.dC_alarm), &Bci_block.dC_alarm });
        config->append({ StartInIndex + 8, sizeof(Bci_block.dTg_pred), &Bci_block.dTg_pred });
        config->append({ StartInIndex + 9, sizeof(Bci_block.dTg_alarm), &Bci_block.dTg_alarm });
        config->append({ StartInIndex + 10, sizeof(Bci_block.dIunb_pred), &Bci_block.dIunb_pred });
        config->append({ StartInIndex + 11, sizeof(Bci_block.dIunb_alarm), &Bci_block.dIunb_alarm });
        config->append({ StartInIndex + 12, sizeof(Bci_block.GdC), &Bci_block.GdC });
        config->append({ StartInIndex + 13, sizeof(Bci_block.GdTg), &Bci_block.GdTg });
        config->append({ StartInIndex + 14, sizeof(Bci_block.GdIunb), &Bci_block.GdIunb });
        config->append({ StartInIndex + 17, sizeof(Bci_block.Iunb_init), &Bci_block.Iunb_init });
        config->append({ StartInIndex + 18, sizeof(Bci_block.Phy_unb_init), &Bci_block.Phy_unb_init });
        config->append({ StartInIndex + 19, sizeof(Bci_block.T_Data_Rec), &Bci_block.T_Data_Rec });
        config->append({ StartInIndex + 20, sizeof(Bci_block.LowU), &Bci_block.LowU });
        config->append({ StartInIndex + 21, sizeof(Bci_block.IsU), &Bci_block.IsU });
        config->append({ StartInIndex + 22, sizeof(Bci_block.IsIunb), &Bci_block.IsIunb });
        config->append({ StartInIndex + 23, sizeof(Bci_block.Tevent_pred), &Bci_block.Tevent_pred });
        config->append({ StartInIndex + 24, sizeof(Bci_block.Tevent_alarm), &Bci_block.Tevent_alarm });
        config->append({ StartInIndex + 27, sizeof(Bci_block.Tg_pasp), &Bci_block.Tg_pasp });
        config->append({ StartInIndex + 28, sizeof(Bci_block.C_pasp), &Bci_block.C_pasp });
        config->append({ 1050, sizeof(Bci_block.Unom_1), &Bci_block.Unom_1 });
    }
    config->append({ 0xFFFFFFFF, 0, nullptr });
}

void ConfigKIV::SetDefConf()
{
    Bci_block.Unom = 220;
    Bci_block.Unom_1 = 100;
    Bci_block.Umin = 0.5;
    Bci_block.Imin = 0.5;
    Bci_block.dC_pred = 0.5;
    Bci_block.dC_alarm = 1.0;
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
    Bci_block.T_Data_Rec = 60;
    Bci_block.LowU = 80;
    Bci_block.IsU = 1;
    Bci_block.IsIunb = 1;
    Bci_block.Tevent_pred = 0;
    Bci_block.Tevent_alarm = 0;

    for (int i = 0; i < 3; i++)
    {

        Bci_block.C_init[i] = 2200;
        Bci_block.Tg_init[i] = 0;
        Bci_block.corTg[i] = 0;
        Bci_block.Tg_pasp[i] = 0.3f;
        Bci_block.C_pasp[i] = 1500;
    }
}
