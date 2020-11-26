#include "configkdv.h"

ConfigKDV::ConfigKDV(S2DataTypes::S2ConfigType *config)
{
    m_S2Config = config;
    Config::removeFotter(config);

    m_S2Config->append({ BCI_EQ_TYPE, sizeof(Bci_block.Eq_type), &Bci_block.Eq_type });
    m_S2Config->append({ BCI_COOL_TYPE, sizeof(Bci_block.Cool_type), &Bci_block.Cool_type });
    m_S2Config->append({ BCI_W_MAT, sizeof(Bci_block.W_mat), &Bci_block.W_mat });
    m_S2Config->append({ BCI_NFILTR, sizeof(Bci_block.NFiltr), &Bci_block.NFiltr });
    m_S2Config->append({ BCI_NHARMFILT, sizeof(Bci_block.NHarmFilt), &Bci_block.NHarmFilt });
    m_S2Config->append({ BCI_DDOCS, sizeof(Bci_block.DDosc), &Bci_block.DDosc });
    m_S2Config->append({ BCI_UNOM1, sizeof(Bci_block.Unom1), &Bci_block.Unom1 });
    m_S2Config->append({ BCI_U2NOM, sizeof(Bci_block.U2nom), &Bci_block.U2nom });
    m_S2Config->append({ BCI_ITT1NOM, sizeof(Bci_block.ITT1nom), &Bci_block.ITT1nom });
    m_S2Config->append({ BCI_ITT2NOM, sizeof(Bci_block.ITT2nom), &Bci_block.ITT2nom });
    m_S2Config->append({ BCI_IWNOM, sizeof(Bci_block.Iwnom), &Bci_block.Iwnom });
    m_S2Config->append({ BCI_DUOSC, sizeof(Bci_block.DUosc), &Bci_block.DUosc });
    m_S2Config->append({ BCI_DIOSC, sizeof(Bci_block.DIosc), &Bci_block.DIosc });
    m_S2Config->append({ BCI_DUIMIN, sizeof(Bci_block.DUImin), &Bci_block.DUImin });
    m_S2Config->append({ BCI_IMIN, sizeof(Bci_block.Imin), &Bci_block.Imin });
    m_S2Config->append({ BCI_TNNTDOP, sizeof(Bci_block.TNNTdop), &Bci_block.TNNTdop });
    m_S2Config->append({ BCI_TNNTPRED, sizeof(Bci_block.TNNTpred), &Bci_block.TNNTpred });
    m_S2Config->append({ BCI_TAMB_NOM, sizeof(Bci_block.Tamb_nom), &Bci_block.Tamb_nom });
    m_S2Config->append({ BCI_DTNNT_NOM, sizeof(Bci_block.dTNNTnom), &Bci_block.dTNNTnom });
    m_S2Config->append({ BCI_KDOB, sizeof(Bci_block.Kdob), &Bci_block.Kdob });
    m_S2Config->append({ BCI_TAUWNOM, sizeof(Bci_block.TauWnom), &Bci_block.TauWnom });
    m_S2Config->append({ BCI_UMAX, sizeof(Bci_block.Umax), &Bci_block.Umax });
    m_S2Config->append({ BCI_IMAX, sizeof(Bci_block.Imax), &Bci_block.Imax });
    m_S2Config->append({ BCI_GTNNT, sizeof(Bci_block.GTnnt), &Bci_block.GTnnt });
    m_S2Config->append({ BCI_GOVC, sizeof(Bci_block.GOvc), &Bci_block.GOvc });
    m_S2Config->append({ BCI_FNOM, sizeof(Bci_block.Fnom), &Bci_block.Fnom });
    m_S2Config->append({ BCI_NOM_SLIP, sizeof(Bci_block.nom_slip), &Bci_block.nom_slip });
    m_S2Config->append({ BCI_UVMAX, sizeof(Bci_block.UVmax), &Bci_block.UVmax });
    m_S2Config->append({ BCI_KVIBR, sizeof(Bci_block.Kvibr), &Bci_block.Kvibr });
    m_S2Config->append({ BCI_VIBRA_PRED, sizeof(Bci_block.VibrA_pred), &Bci_block.VibrA_pred });
    m_S2Config->append({ BCI_VIBRV_PRED, sizeof(Bci_block.VibrV_pred), &Bci_block.VibrV_pred });
    m_S2Config->append({ BCI_VIBRD_PRED, sizeof(Bci_block.VibrD_pred), &Bci_block.VibrD_pred });
    m_S2Config->append({ BCI_VIBRA_ALARM, sizeof(Bci_block.VibrA_alarm), &Bci_block.VibrA_alarm });
    m_S2Config->append({ BCI_VIBRV_ALARM, sizeof(Bci_block.VibrV_alarm), &Bci_block.VibrV_alarm });
    m_S2Config->append({ BCI_VIBRD_ALARM, sizeof(Bci_block.VibrD_alarm), &Bci_block.VibrD_alarm });
    m_S2Config->append({ BCI_VVIBRA_PRED, sizeof(Bci_block.VVibrA_pred), &Bci_block.VVibrA_pred });
    m_S2Config->append({ BCI_VVIBRV_PRED, sizeof(Bci_block.VVibrV_pred), &Bci_block.VVibrV_pred });
    m_S2Config->append({ BCI_VVIBRD_PRED, sizeof(Bci_block.VVibrD_pred), &Bci_block.VVibrD_pred });
    m_S2Config->append({ BCI_VVIBRA_ALARM, sizeof(Bci_block.VVibrA_alarm), &Bci_block.VVibrA_alarm });
    m_S2Config->append({ BCI_VVIBRV_ALARM, sizeof(Bci_block.VVibrV_alarm), &Bci_block.VVibrV_alarm });
    m_S2Config->append({ BCI_VVIBRD_ALARM, sizeof(Bci_block.VVibrD_alarm), &Bci_block.VVibrD_alarm });
    m_S2Config->append({ BCI_NUMA, sizeof(Bci_block.NumA), &Bci_block.NumA });
    m_S2Config->append({ BCI_POLES, sizeof(Bci_block.Poles), &Bci_block.Poles });
    m_S2Config->append({ BCI_STATOR_SLOTES, sizeof(Bci_block.Stator_Slotes), &Bci_block.Stator_Slotes });
    m_S2Config->append({ BCI_ROTOR_BARS, sizeof(Bci_block.Rotor_bars), &Bci_block.Rotor_bars });
    m_S2Config->append({ BCI_VIBROTYPE, sizeof(Bci_block.VibroType), &Bci_block.VibroType });
    m_S2Config->append({ BCI_SENSORS, sizeof(Bci_block.Sensors), &Bci_block.Sensors });
    m_S2Config->append({ BCI_T_DATA_REC, sizeof(Bci_block.T_Data_Rec), &Bci_block.T_Data_Rec });
    m_S2Config->append({ BCI_OSCPOINTS, sizeof(Bci_block.OscPoints), &Bci_block.OscPoints });
    m_S2Config->append({ BCI_TDATNUM, sizeof(Bci_block.TdatNum), &Bci_block.TdatNum });

    m_S2Config->append({ 0xFFFFFFFF, 0, nullptr });
}

S2DataTypes::S2ConfigType *ConfigKDV::S2Config()
{
    return m_S2Config;
}

void ConfigKDV::SetDefConf()
{

    Bci_block.Eq_type = DEF_EQ_TYPE;
    Bci_block.Cool_type = DEF_COOL_TYPE;
    Bci_block.W_mat = DEF_W_MAT;
    Bci_block.NFiltr = DEF_NFILTR;
    Bci_block.NHarmFilt = DEF_NHARMFILT;
    Bci_block.DDosc = DEF_DDOCS;
    Bci_block.Unom1 = static_cast<float>(DEF_UNOM1);
    Bci_block.U2nom = DEF_U2NOM;
    Bci_block.ITT1nom = DEF_ITT1NOM;
    Bci_block.ITT2nom = DEF_ITT2NOM;
    Bci_block.Iwnom = DEF_IWNOM;
    Bci_block.DUosc = DEF_DUOSC;
    Bci_block.DIosc = DEF_DIOSC;
    Bci_block.DUImin = DEF_DUIMIN;
    Bci_block.Imin = static_cast<float>(DEF_IMIN);
    Bci_block.TNNTdop = DEF_TNNTDOP;
    Bci_block.TNNTpred = DEF_TNNTPRED;
    Bci_block.Tamb_nom = DEF_TAMB_NOM;
    Bci_block.dTNNTnom = DEF_DTNNT_NOM;
    Bci_block.Kdob = static_cast<float>(DEF_KDOB);
    Bci_block.TauWnom = DEF_TAUWNOM;
    Bci_block.Umax = DEF_UMAX;
    Bci_block.Imax = DEF_IMAX;
    Bci_block.GTnnt = DEF_GTNNT;
    Bci_block.GOvc = DEF_GOVC;
    Bci_block.Fnom = DEF_FNOM;
    Bci_block.nom_slip = static_cast<float>(DEF_NOM_SLIP);
    Bci_block.UVmax = DEF_UVMAX;
    Bci_block.Kvibr = static_cast<float>(DEF_KVIBR);
    Bci_block.VibrA_pred = DEF_VIBRA_PRED;
    Bci_block.VibrV_pred = DEF_VIBRV_PRED;
    Bci_block.VibrD_pred = DEF_VIBRD_PRED;
    Bci_block.VibrA_alarm = static_cast<float>(DEF_VIBRA_ALARM);
    Bci_block.VibrV_alarm = static_cast<float>(DEF_VIBRV_ALARM);
    Bci_block.VibrD_alarm = DEF_VIBRD_ALARM;
    Bci_block.VVibrA_pred = static_cast<float>(DEF_VVIBRA_PRED);
    Bci_block.VVibrV_pred = DEF_VVIBRV_PRED;
    Bci_block.VVibrD_pred = DEF_VVIBRD_PRED;
    Bci_block.VVibrA_alarm = static_cast<float>(DEF_VVIBRA_ALARM);
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
