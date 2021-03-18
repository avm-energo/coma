#include "configkdv.h"

#include "../gen/s2.h"

ConfigKDV::ConfigKDV()
{
    m_mainConfig = new Config;
    m_KxxConfig = new ConfigKxx;
}

void ConfigKDV::SetDefConf()
{
    m_mainConfig->SetDefConf();
    m_KxxConfig->SetDefConf();
    using namespace AVM_KDV;
    using namespace DataTypes;
    // Bci_block.Eq_type = DEF_EQ_TYPE;
    // Bci_block.Cool_type = DEF_COOL_TYPE;
    // Bci_block.W_mat = DEF_W_MAT;
    S2::setRecordValue({ BciNumber::NFiltr_ID, DEF_NFILTR });
    S2::setRecordValue({ BciNumber::NHarmFilt_ID, DEF_NHARMFILT });
    S2::setRecordValue({ BciNumber::DDOsc_ID, DEF_DDOCS });
    S2::setRecordValue({ BciNumber::Unom1, DEF_UNOM1 });
    S2::setRecordValue({ BciNumber::U2nom, DEF_U2NOM });
    S2::setRecordValue({ BciNumber::ITT1nom_KTF_KDV, DEF_ITT1NOM });
    S2::setRecordValue({ BciNumber::ITT2nom_KTF_KDV, DEF_ITT2NOM });
    S2::setRecordValue({ BciNumber::Iwnom, DEF_IWNOM });
    S2::setRecordValue({ BciNumber::DUosc, DEF_DUOSC });
    S2::setRecordValue({ BciNumber::DIosc_ID, DEF_DIOSC });
    S2::setRecordValue({ BciNumber::DUImin_ID, DEF_DUIMIN });
    S2::setRecordValue({ BciNumber::Imin, DEF_IMIN });
    S2::setRecordValue({ BciNumber::TNNTdop, DEF_TNNTDOP });
    S2::setRecordValue({ BciNumber::TNNTpred, DEF_TNNTPRED });
    S2::setRecordValue({ BciNumber::Tamb_nom, DEF_TAMB_NOM });
    S2::setRecordValue({ BciNumber::dTNNTnom, DEF_DTNNT_NOM });
    S2::setRecordValue({ BciNumber::Kdob, DEF_KDOB });
    S2::setRecordValue({ BciNumber::TauWnom, DEF_TAUWNOM });
    S2::setRecordValue({ BciNumber::Umaxm, DEF_UMAX });
    S2::setRecordValue({ BciNumber::Imaxm, DEF_IMAX });
    S2::setRecordValue({ BciNumber::GTnnt, DEF_GTNNT });
    S2::setRecordValue({ BciNumber::GOvc, DEF_GOVC });
    S2::setRecordValue({ BciNumber::Fnom, DEF_FNOM });
    S2::setRecordValue({ BciNumber::nom_slip, DEF_NOM_SLIP });
    S2::setRecordValue({ BciNumber::UVmax, DEF_UVMAX });

    S2::setRecordValue({ BciNumber::Kvibr, DEF_KVIBR });
    S2::setRecordValue({ BciNumber::VibrA_pred, DEF_VIBRA_PRED });
    S2::setRecordValue({ BciNumber::VibrV_pred, DEF_VIBRV_PRED });
    S2::setRecordValue({ BciNumber::VibrD_pred, DEF_VIBRD_PRED });
    S2::setRecordValue({ BciNumber::VibrA_alarm, DEF_VIBRA_ALARM });
    S2::setRecordValue({ BciNumber::VibrV_alarm, DEF_VIBRV_ALARM });
    S2::setRecordValue({ BciNumber::VibrD_alarm, DEF_VIBRD_ALARM });
    S2::setRecordValue({ BciNumber::VVibrA_pred, DEF_VVIBRA_PRED });
    S2::setRecordValue({ BciNumber::VVibrV_pred, DEF_VVIBRV_PRED });
    S2::setRecordValue({ BciNumber::VVibrD_pred, DEF_VVIBRD_PRED });
    S2::setRecordValue({ BciNumber::VVibrA_alarm, DEF_VVIBRA_ALARM });
    S2::setRecordValue({ BciNumber::VVibrV_alarm, DEF_VVIBRV_ALARM });
    S2::setRecordValue({ BciNumber::VVibrD_alarm, DEF_VVIBRD_ALARM });

    S2::setRecordValue({ BciNumber::NumA_KDV, DEF_NUMA });
    S2::setRecordValue({ BciNumber::Poles, DEF_POLES });
    S2::setRecordValue({ BciNumber::Stator_Slotes, DEF_STATOR_SLOTES });
    S2::setRecordValue({ BciNumber::Rotor_bars, DEF_ROTOR_BARS });
    S2::setRecordValue({ BciNumber::VibroType, DEF_VIBROTYPE });
    S2::setRecordValue({ BciNumber::Sensors, DEF_SENSORS });
    S2::setRecordValue({ BciNumber::T_Data_Rec, DEF_T_DATA_REC });
    S2::setRecordValue({ BciNumber::OscPoints, DEF_OSCPOINTS });
    S2::setRecordValue({ BciNumber::TdatNum, DEF_TDATNUM });
}

Config *ConfigKDV::MainConfig()
{
    return m_mainConfig;
}

ConfigKxx *ConfigKDV::KxxConfig()
{
    return m_KxxConfig;
}
