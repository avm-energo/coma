#include "configktf.h"

#include "../gen/s2.h"
#include "config.h"

ConfigKTF::ConfigKTF()
{

    m_mainConfig = new Config;
    m_KxxConfig = new ConfigKxx;
}

ConfigKTF::~ConfigKTF()
{
    delete m_mainConfig;
    delete m_KxxConfig;
}

void ConfigKTF::SetDefConf()
{
    using namespace AVM_KTF;
    using namespace DataTypes;
    m_mainConfig->SetDefConf();
    m_KxxConfig->SetDefConf();

    S2::setRecordValue({ BciNumber::NFiltr_ID, DEF_NFILTR });
    S2::setRecordValue({ BciNumber::NHarmFilt_ID, DEF_NHARMFILT });
    S2::setRecordValue({ BciNumber::DDOsc_ID, DEF_DDOCS });
    S2::setRecordValue({ BciNumber::Unom1, DEF_UNOM1KTF });
    S2::setRecordValue({ BciNumber::DUosc, DEF_DUOSC });
    S2::setRecordValue({ BciNumber::DIosc_ID, DEF_DIOSC });
    S2::setRecordValue({ BciNumber::DUImin_ID, DEF_DUIMIN });
    S2::setRecordValue({ BciNumber::Imin, DEF_IMIN });
    S2::setRecordValue({ BciNumber::T_Data_Rec, DEF_T_DATA_RECKTF });
    S2::setRecordValue({ BciNumber::U2nom, DEF_U2NOM });
    S2::setRecordValue({ BciNumber::ITT1nom_KTF_KDV, DEF_ITT1NOM });
    S2::setRecordValue({ BciNumber::ITT2nom_KTF_KDV, DEF_ITT2NOM });
    S2::setRecordValue({ BciNumber::Cool_type, DEF_COOL_TYPE });
    S2::setRecordValue({ BciNumber::W_mat, DEF_W_MAT });
    S2::setRecordValue({ BciNumber::TNNTdop, DEF_TNNTDOP });
    S2::setRecordValue({ BciNumber::TNNTpred, DEF_TNNTPRED });
    S2::setRecordValue({ BciNumber::Tamb_nom, DEF_TAMB_NOM });
    S2::setRecordValue({ BciNumber::dTNNTnom, DEF_DTNNT_NOM });
    S2::setRecordValue({ BciNumber::Kdob, DEF_KDOB });
    S2::setRecordValue({ BciNumber::TauWnom, DEF_TAUWNOM });
    S2::setRecordValue({ BciNumber::Umaxm, DEF_UMAX });
    S2::setRecordValue({ BciNumber::Imaxm, DEF_IMAX });
    S2::setRecordValue({ BciNumber::Iwnom, DEF_IWNOM });
    S2::setRecordValue({ BciNumber::OscPoints, DEF_OSCPOINTS });
    S2::setRecordValue({ BciNumber::GTnnt, DEF_GTNNT });
    S2::setRecordValue({ BciNumber::GOvc, DEF_GOVC });
    S2::setRecordValue({ BciNumber::TdatNum, DEF_TDATNUM });
}

Config *ConfigKTF::MainConfig()
{
    return m_mainConfig;
}

ConfigKxx *ConfigKTF::KxxConfig()
{
    return m_KxxConfig;
}
