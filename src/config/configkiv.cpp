#include "configkiv.h"

#include "../gen/s2.h"
ConfigKIV::ConfigKIV()
{

    m_mainConfig = new Config;
    m_KxxConfig = new ConfigKxx;
    // параметры входных сигналов
    // setConfig();
}

ConfigKIV::~ConfigKIV()
{
    delete m_mainConfig;
    delete m_KxxConfig;
}

void ConfigKIV::setDefConf()
{
    m_mainConfig->SetDefConf();
    m_KxxConfig->SetDefConf();
    using namespace DataTypes;
    S2::setRecordValue({ BciNumber::Unom1, float(220) });
    S2::setRecordValue({ BciNumber::DUImin_ID, float(0.5) });
    S2::setRecordValue({ BciNumber::Imin, float(0.5) });

    S2::setRecordValue({ BciNumber::Tg_Pasp_ID, FLOAT_3t { 0.3f, 0.3f, 0.3f } });

    S2::setRecordValue({ BciNumber::C_Pasp_ID, FLOAT_3t { 1500, 1500, 1500 } });

    S2::setRecordValue({ BciNumber::dC_pred, float(0.5) });
    S2::setRecordValue({ BciNumber::dC_alarm, float(1.0) });
    S2::setRecordValue({ BciNumber::dTg_pred, float(1.0) });
    S2::setRecordValue({ BciNumber::dTg_alarm, float(1.5) });
    S2::setRecordValue({ BciNumber::dIunb_pred, float(1.0) });
    S2::setRecordValue({ BciNumber::dIunb_alarm, float(1.5) });

    S2::setRecordValue({ BciNumber::NFiltr_ID, DWORD(100) });
    S2::setRecordValue({ BciNumber::T_Data_Rec, DWORD(60) });

    S2::setRecordValue({ BciNumber::U2nom, float(100.0) });
    S2::setRecordValue({ BciNumber::LowU, float(80.0) });

    S2::setRecordValue({ BciNumber::Tevent_pred, DWORD(0) });
    S2::setRecordValue({ BciNumber::Tevent_alarm, DWORD(0) });

    S2::setRecordValue({ BciNumber::IsU, BYTE(true) });
    S2::setRecordValue({ BciNumber::IsIuIunb, BYTE(true) });
    //    Bci_block.Unom1 = 220;
    //    Bci_block.U2nom = 100;
    //    Bci_block.Umin = 0.5;
    //    Bci_block.Imin = 0.5;
    //    Bci_block.dC_pred = 0.5;
    //    Bci_block.dC_alarm = 1.0;
    //    Bci_block.dTg_pred = 1;
    //    Bci_block.dTg_alarm = 1.5;
    //    Bci_block.dIunb_pred = 1;
    //    Bci_block.dIunb_alarm = 1.5;
    //    Bci_block.GdC = 0.1f;
    //    Bci_block.GdTg = 0.2f;
    //    Bci_block.GdIunb = 0.2f;
    //    Bci_block.Iunb_init = 0;
    //    Bci_block.Phy_unb_init = 0;
    //    Bci_block.NFiltr = 100;
    //    Bci_block.T_Data_Rec = 60;
    //    Bci_block.LowU = 80;
    //    Bci_block.IsU = 1;
    //    Bci_block.IsIunb = 1;
    //    Bci_block.Tevent_pred = 0;
    //    Bci_block.Tevent_alarm = 0;

    //    for (int i = 0; i < 3; i++)
    //    {
    //        Bci_block.Tg_pasp[i] = 0.3f;
    //        Bci_block.C_pasp[i] = 1500;
    //    }
}

Config *ConfigKIV::MainConfig()
{
    return m_mainConfig;
}

ConfigKxx *ConfigKIV::KxxConfig()
{
    return m_KxxConfig;
}

// void ConfigKIV::setConfig()
//{
//    Config::removeFotter();

//    S2::config.append({ BciNumber::NFiltr_ID, sizeof(Bci_block.NFiltr), &Bci_block.NFiltr });
//    S2::config.append({ BciNumber::Unom1, sizeof(Bci_block.Unom1), &Bci_block.Unom1 });
//    S2::config.append({ BciNumber::DUImin_ID, sizeof(Bci_block.Umin), &Bci_block.Umin });
//    S2::config.append({ BciNumber::Imin, sizeof(Bci_block.Imin), &Bci_block.Imin });
//    S2::config.append({ BciNumber::dC_pred, sizeof(Bci_block.dC_pred), &Bci_block.dC_pred });
//    S2::config.append({ BciNumber::dC_alarm, sizeof(Bci_block.dC_alarm), &Bci_block.dC_alarm });
//    S2::config.append({ BciNumber::dTg_pred, sizeof(Bci_block.dTg_pred), &Bci_block.dTg_pred });
//    S2::config.append({ BciNumber::dTg_alarm, sizeof(Bci_block.dTg_alarm), &Bci_block.dTg_alarm });
//    S2::config.append({ BciNumber::dIunb_pred, sizeof(Bci_block.dIunb_pred), &Bci_block.dIunb_pred });
//    S2::config.append({ BciNumber::dIunb_alarm, sizeof(Bci_block.dIunb_alarm), &Bci_block.dIunb_alarm });
//    S2::config.append({ BciNumber::GdC, sizeof(Bci_block.GdC), &Bci_block.GdC });
//    S2::config.append({ BciNumber::GdTg, sizeof(Bci_block.GdTg), &Bci_block.GdTg });
//    S2::config.append({ BciNumber::GdIunb, sizeof(Bci_block.GdIunb), &Bci_block.GdIunb });
//    S2::config.append({ BciNumber::Iunb_init, sizeof(Bci_block.Iunb_init), &Bci_block.Iunb_init });
//    S2::config.append({ BciNumber::Phy_unb_init, sizeof(Bci_block.Phy_unb_init), &Bci_block.Phy_unb_init });
//    S2::config.append({ BciNumber::T_Data_Rec, sizeof(Bci_block.T_Data_Rec), &Bci_block.T_Data_Rec });
//    S2::config.append({ BciNumber::LowU, sizeof(Bci_block.LowU), &Bci_block.LowU });
//    S2::config.append({ BciNumber::IsU, sizeof(Bci_block.IsU), &Bci_block.IsU });
//    S2::config.append({ BciNumber::IsIuIunb, sizeof(Bci_block.IsIunb), &Bci_block.IsIunb });
//    S2::config.append({ BciNumber::Tevent_pred, sizeof(Bci_block.Tevent_pred), &Bci_block.Tevent_pred });
//    S2::config.append({ BciNumber::Tevent_alarm, sizeof(Bci_block.Tevent_alarm), &Bci_block.Tevent_alarm });
//    S2::config.append({ BciNumber::Tg_Pasp_ID, sizeof(Bci_block.Tg_pasp), &Bci_block.Tg_pasp });
//    S2::config.append({ BciNumber::C_Pasp_ID, sizeof(Bci_block.C_pasp), &Bci_block.C_pasp });
//    S2::config.append({ BciNumber::U2nom, sizeof(Bci_block.U2nom), &Bci_block.U2nom });
//    S2::config.append({ 0xFFFFFFFF, 0, nullptr });
//}
