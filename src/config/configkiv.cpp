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
}

Config *ConfigKIV::MainConfig()
{
    return m_mainConfig;
}

ConfigKxx *ConfigKIV::KxxConfig()
{
    return m_KxxConfig;
}
