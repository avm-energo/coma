#include "configkdv.h"

#include "../gen/s2.h"

ConfigKDV::ConfigKDV()
{
    m_mainConfig = new Config;
    m_KxxConfig = new ConfigKxx;
    setConfig();
}

void ConfigKDV::setConfig()
{
    Config::removeFotter();

    S2::config.append({ BciNumber::Eq_type_ID, sizeof(Bci_block.Eq_type), &Bci_block.Eq_type });
    S2::config.append({ BciNumber::Cool_type, sizeof(Bci_block.Cool_type), &Bci_block.Cool_type });
    S2::config.append({ BciNumber::W_mat, sizeof(Bci_block.W_mat), &Bci_block.W_mat });
    S2::config.append({ BciNumber::NFiltr_ID, sizeof(Bci_block.NFiltr), &Bci_block.NFiltr });
    S2::config.append({ BciNumber::NHarmFilt_ID, sizeof(Bci_block.NHarmFilt), &Bci_block.NHarmFilt });
    S2::config.append({ BciNumber::DDOsc_ID, sizeof(Bci_block.DDosc), &Bci_block.DDosc });
    S2::config.append({ BciNumber::Unom1, sizeof(Bci_block.Unom1), &Bci_block.Unom1 });
    S2::config.append({ BciNumber::U2nom, sizeof(Bci_block.U2nom), &Bci_block.U2nom });
    S2::config.append({ BciNumber::ITT1nom_KTF_KDV, sizeof(Bci_block.ITT1nom), &Bci_block.ITT1nom });
    S2::config.append({ BciNumber::ITT2nom_KTF_KDV, sizeof(Bci_block.ITT2nom), &Bci_block.ITT2nom });
    S2::config.append({ BciNumber::Iwnom, sizeof(Bci_block.Iwnom), &Bci_block.Iwnom });
    S2::config.append({ BciNumber::DUosc, sizeof(Bci_block.DUosc), &Bci_block.DUosc });
    S2::config.append({ BciNumber::DIosc_ID, sizeof(Bci_block.DIosc), &Bci_block.DIosc });
    S2::config.append({ BciNumber::DUImin_ID, sizeof(Bci_block.DUImin), &Bci_block.DUImin });
    S2::config.append({ BciNumber::Imin, sizeof(Bci_block.Imin), &Bci_block.Imin });
    S2::config.append({ BciNumber::TNNTdop, sizeof(Bci_block.TNNTdop), &Bci_block.TNNTdop });
    S2::config.append({ BciNumber::TNNTpred, sizeof(Bci_block.TNNTpred), &Bci_block.TNNTpred });
    S2::config.append({ BciNumber::Tamb_nom, sizeof(Bci_block.Tamb_nom), &Bci_block.Tamb_nom });
    S2::config.append({ BciNumber::dTNNTnom, sizeof(Bci_block.dTNNTnom), &Bci_block.dTNNTnom });
    S2::config.append({ BciNumber::Kdob, sizeof(Bci_block.Kdob), &Bci_block.Kdob });
    S2::config.append({ BciNumber::TauWnom, sizeof(Bci_block.TauWnom), &Bci_block.TauWnom });
    S2::config.append({ BciNumber::Umaxm, sizeof(Bci_block.Umax), &Bci_block.Umax });
    S2::config.append({ BciNumber::Imaxm, sizeof(Bci_block.Imax), &Bci_block.Imax });
    S2::config.append({ BciNumber::GTnnt, sizeof(Bci_block.GTnnt), &Bci_block.GTnnt });
    S2::config.append({ BciNumber::GOvc, sizeof(Bci_block.GOvc), &Bci_block.GOvc });
    S2::config.append({ BciNumber::Fnom, sizeof(Bci_block.Fnom), &Bci_block.Fnom });
    S2::config.append({ BciNumber::nom_slip, sizeof(Bci_block.nom_slip), &Bci_block.nom_slip });
    S2::config.append({ BciNumber::UVmax, sizeof(Bci_block.UVmax), &Bci_block.UVmax });
    S2::config.append({ BciNumber::Kvibr, sizeof(Bci_block.Kvibr), &Bci_block.Kvibr });
    S2::config.append({ BciNumber::VibrA_pred, sizeof(Bci_block.VibrA_pred), &Bci_block.VibrA_pred });
    S2::config.append({ BciNumber::VibrV_pred, sizeof(Bci_block.VibrV_pred), &Bci_block.VibrV_pred });
    S2::config.append({ BciNumber::VibrD_pred, sizeof(Bci_block.VibrD_pred), &Bci_block.VibrD_pred });
    S2::config.append({ BciNumber::VibrA_alarm, sizeof(Bci_block.VibrA_alarm), &Bci_block.VibrA_alarm });
    S2::config.append({ BciNumber::VibrV_alarm, sizeof(Bci_block.VibrV_alarm), &Bci_block.VibrV_alarm });
    S2::config.append({ BciNumber::VibrD_alarm, sizeof(Bci_block.VibrD_alarm), &Bci_block.VibrD_alarm });
    S2::config.append({ BciNumber::VVibrA_pred, sizeof(Bci_block.VVibrA_pred), &Bci_block.VVibrA_pred });
    S2::config.append({ BciNumber::VVibrV_pred, sizeof(Bci_block.VVibrV_pred), &Bci_block.VVibrV_pred });
    S2::config.append({ BciNumber::VVibrD_pred, sizeof(Bci_block.VVibrD_pred), &Bci_block.VVibrD_pred });
    S2::config.append({ BciNumber::VVibrA_alarm, sizeof(Bci_block.VVibrA_alarm), &Bci_block.VVibrA_alarm });
    S2::config.append({ BciNumber::VVibrV_alarm, sizeof(Bci_block.VVibrV_alarm), &Bci_block.VVibrV_alarm });
    S2::config.append({ BciNumber::VVibrD_alarm, sizeof(Bci_block.VVibrD_alarm), &Bci_block.VVibrD_alarm });
    S2::config.append({ BciNumber::NumA_KDV, sizeof(Bci_block.NumA), &Bci_block.NumA });
    S2::config.append({ BciNumber::Poles, sizeof(Bci_block.Poles), &Bci_block.Poles });
    S2::config.append({ BciNumber::Stator_Slotes, sizeof(Bci_block.Stator_Slotes), &Bci_block.Stator_Slotes });
    S2::config.append({ BciNumber::Rotor_bars, sizeof(Bci_block.Rotor_bars), &Bci_block.Rotor_bars });
    S2::config.append({ BciNumber::VibroType, sizeof(Bci_block.VibroType), &Bci_block.VibroType });
    S2::config.append({ BciNumber::Sensors, sizeof(Bci_block.Sensors), &Bci_block.Sensors });
    S2::config.append({ BciNumber::T_Data_Rec, sizeof(Bci_block.T_Data_Rec), &Bci_block.T_Data_Rec });
    S2::config.append({ BciNumber::OscPoints, sizeof(Bci_block.OscPoints), &Bci_block.OscPoints });
    S2::config.append({ BciNumber::TdatNum, sizeof(Bci_block.TdatNum), &Bci_block.TdatNum });

    S2::config.append({ 0xFFFFFFFF, 0, nullptr });
}

// S2DataTypes::S2::configType *ConfigKDV::S2::config()
//{
//    return m_S2::config;
//}

void ConfigKDV::SetDefConf()
{
    using namespace AVM_KDV;
    Bci_block.Eq_type = DEF_EQ_TYPE;
    Bci_block.Cool_type = DEF_COOL_TYPE;
    Bci_block.W_mat = DEF_W_MAT;
    Bci_block.NFiltr = DEF_NFILTR;
    Bci_block.NHarmFilt = DEF_NHARMFILT;
    Bci_block.DDosc = DEF_DDOCS;
    Bci_block.Unom1 = DEF_UNOM1;
    Bci_block.U2nom = DEF_U2NOM;
    Bci_block.ITT1nom = DEF_ITT1NOM;
    Bci_block.ITT2nom = DEF_ITT2NOM;
    Bci_block.Iwnom = DEF_IWNOM;
    Bci_block.DUosc = DEF_DUOSC;
    Bci_block.DIosc = DEF_DIOSC;
    Bci_block.DUImin = DEF_DUIMIN;
    Bci_block.Imin = DEF_IMIN;
    Bci_block.TNNTdop = DEF_TNNTDOP;
    Bci_block.TNNTpred = DEF_TNNTPRED;
    Bci_block.Tamb_nom = DEF_TAMB_NOM;
    Bci_block.dTNNTnom = DEF_DTNNT_NOM;
    Bci_block.Kdob = DEF_KDOB;
    Bci_block.TauWnom = DEF_TAUWNOM;
    Bci_block.Umax = DEF_UMAX;
    Bci_block.Imax = DEF_IMAX;
    Bci_block.GTnnt = DEF_GTNNT;
    Bci_block.GOvc = DEF_GOVC;
    Bci_block.Fnom = DEF_FNOM;
    Bci_block.nom_slip = DEF_NOM_SLIP;
    Bci_block.UVmax = DEF_UVMAX;
    Bci_block.Kvibr = DEF_KVIBR;
    Bci_block.VibrA_pred = DEF_VIBRA_PRED;
    Bci_block.VibrV_pred = DEF_VIBRV_PRED;
    Bci_block.VibrD_pred = DEF_VIBRD_PRED;
    Bci_block.VibrA_alarm = DEF_VIBRA_ALARM;
    Bci_block.VibrV_alarm = DEF_VIBRV_ALARM;
    Bci_block.VibrD_alarm = DEF_VIBRD_ALARM;
    Bci_block.VVibrA_pred = DEF_VVIBRA_PRED;
    Bci_block.VVibrV_pred = DEF_VVIBRV_PRED;
    Bci_block.VVibrD_pred = DEF_VVIBRD_PRED;
    Bci_block.VVibrA_alarm = DEF_VVIBRA_ALARM;
    Bci_block.VVibrV_alarm = DEF_VVIBRV_ALARM;
    Bci_block.VVibrD_alarm = DEF_VVIBRD_ALARM;
    Bci_block.NumA = DEF_NUMA;
    Bci_block.Poles = DEF_POLES;
    Bci_block.Stator_Slotes = DEF_STATOR_SLOTES;
    Bci_block.Rotor_bars = DEF_ROTOR_BARS;
    Bci_block.VibroType = DEF_VIBROTYPE;
    Bci_block.Sensors = DEF_SENSORS;
    Bci_block.T_Data_Rec = DEF_T_DATA_REC;
    Bci_block.OscPoints = DEF_OSCPOINTS;
    Bci_block.TdatNum = DEF_TDATNUM;
}

Config *ConfigKDV::MainConfig()
{
    return m_mainConfig;
}

ConfigKxx *ConfigKDV::KxxConfig()
{
    return m_KxxConfig;
}
