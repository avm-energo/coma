#include "configktf.h"

#include "../gen/s2.h"
#include "config.h"

ConfigKTF::ConfigKTF()
{

    m_mainConfig = new Config;
    m_KxxConfig = new ConfigKxx;

    Config::removeFotter();

    S2::config.append({ BciNumber::NFiltr_ID, sizeof(Bci_block.NFiltr), &Bci_block.NFiltr });
    S2::config.append({ BciNumber::NHarmFilt_ID, sizeof(Bci_block.NHarmFilt), &Bci_block.NHarmFilt });
    S2::config.append({ BciNumber::DDOsc_ID, sizeof(Bci_block.DDosc), &Bci_block.DDosc });
    S2::config.append({ BciNumber::Unom1, sizeof(Bci_block.Unom1), &Bci_block.Unom1 });
    S2::config.append({ BciNumber::DUosc, sizeof(Bci_block.DUosc), &Bci_block.DUosc });
    S2::config.append({ BciNumber::DIosc_ID, sizeof(Bci_block.DIosc), &Bci_block.DIosc });
    S2::config.append({ BciNumber::DUImin_ID, sizeof(Bci_block.DUImin), &Bci_block.DUImin });
    S2::config.append({ BciNumber::Imin, sizeof(Bci_block.Imin), &Bci_block.Imin });
    S2::config.append({ BciNumber::T_Data_Rec, sizeof(Bci_block.T_Data_Rec), &Bci_block.T_Data_Rec });
    S2::config.append({ BciNumber::U2nom, sizeof(Bci_block.U2nom), &Bci_block.U2nom });
    S2::config.append({ BciNumber::ITT1nom_KTF_KDV, sizeof(Bci_block.ITT1nom), &Bci_block.ITT1nom });
    S2::config.append({ BciNumber::ITT2nom_KTF_KDV, sizeof(Bci_block.ITT2nom), &Bci_block.ITT2nom });
    S2::config.append({ BciNumber::Cool_type, sizeof(Bci_block.Cool_type), &Bci_block.Cool_type });
    S2::config.append({ BciNumber::W_mat, sizeof(Bci_block.W_mat), &Bci_block.W_mat });
    S2::config.append({ BciNumber::TNNTdop, sizeof(Bci_block.TNNTdop), &Bci_block.TNNTdop });
    S2::config.append({ BciNumber::TNNTpred, sizeof(Bci_block.TNNTpred), &Bci_block.TNNTpred });
    S2::config.append({ BciNumber::Tamb_nom, sizeof(Bci_block.Tamb_nom), &Bci_block.Tamb_nom });
    S2::config.append({ BciNumber::dTNNTnom, sizeof(Bci_block.dTNNTnom), &Bci_block.dTNNTnom });
    S2::config.append({ BciNumber::Kdob, sizeof(Bci_block.Kdob), &Bci_block.Kdob });
    S2::config.append({ BciNumber::TauWnom, sizeof(Bci_block.TauWnom), &Bci_block.TauWnom });
    S2::config.append({ BciNumber::Umaxm, sizeof(Bci_block.Umax), &Bci_block.Umax });
    S2::config.append({ BciNumber::Imaxm, sizeof(Bci_block.Imax), &Bci_block.Imax });
    S2::config.append({ BciNumber::Iwnom, sizeof(Bci_block.Iwnom), &Bci_block.Iwnom });
    S2::config.append({ BciNumber::OscPoints, sizeof(Bci_block.OscPoints), &Bci_block.OscPoints });
    S2::config.append({ BciNumber::GTnnt, sizeof(Bci_block.GTnnt), &Bci_block.GTnnt });
    S2::config.append({ BciNumber::GOvc, sizeof(Bci_block.GOvc), &Bci_block.GOvc });
    S2::config.append({ BciNumber::TdatNum, sizeof(Bci_block.TdatNum), &Bci_block.TdatNum });

    S2::config.append({ 0xFFFFFFFF, 0, nullptr });
}

// S2DataTypes::S2ConfigType *ConfigKTF::S2Config()
//{
//    return m_S2Config;
//}

ConfigKTF::~ConfigKTF()
{
    delete m_mainConfig;
    delete m_KxxConfig;
}

void ConfigKTF::SetDefConf()
{

    m_mainConfig->SetDefConf();
    m_KxxConfig->SetDefConf();
    Bci_block.NFiltr = DEF_NFILTR;
    Bci_block.NHarmFilt = DEF_NHARMFILT;
    Bci_block.DDosc = DEF_DDOCS;
    Bci_block.Unom1 = DEF_UNOM1KTF;
    Bci_block.DUosc = DEF_DUOSC;
    Bci_block.DIosc = DEF_DIOSC;
    Bci_block.DUImin = DEF_DUIMIN;
    Bci_block.Imin = static_cast<float>(DEF_IMIN);
    Bci_block.T_Data_Rec = DEF_T_DATA_RECKTF;
    Bci_block.U2nom = DEF_U2NOM;
    Bci_block.ITT1nom = DEF_ITT1NOM;
    Bci_block.ITT2nom = DEF_ITT2NOM;
    Bci_block.Cool_type = DEF_COOL_TYPE;
    Bci_block.W_mat = DEF_W_MAT;
    Bci_block.TNNTdop = DEF_TNNTDOP;
    Bci_block.TNNTpred = DEF_TNNTPRED;
    Bci_block.Tamb_nom = DEF_TAMB_NOM;
    Bci_block.dTNNTnom = DEF_DTNNT_NOM;
    Bci_block.Kdob = static_cast<float>(DEF_KDOB);
    Bci_block.TauWnom = DEF_TAUWNOM;
    Bci_block.Umax = DEF_UMAX;
    Bci_block.Imax = DEF_IMAX;
    Bci_block.Iwnom = DEF_IWNOM;
    Bci_block.OscPoints = DEF_OSCPOINTS;
    Bci_block.GTnnt = DEF_GTNNT;
    Bci_block.GOvc = DEF_GOVC;
    Bci_block.TdatNum = DEF_TDATNUM;
}

Config *ConfigKTF::MainConfig()
{
    return m_mainConfig;
}

ConfigKxx *ConfigKTF::KxxConfig()
{
    return m_KxxConfig;
}
