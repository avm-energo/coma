#ifndef CONFIGKTF_H
#define CONFIGKTF_H

#include "../gen/datatypes.h"
#include "../module/modules.h"
#include "config.h"
#include "configkxx.h"

namespace AVM_KTF
{
constexpr DataTypes::DWORD DEF_NFILTR = 100.0f;
constexpr DataTypes::DWORD DEF_NHARMFILT = 3.0f;
constexpr DataTypes::DWORD DEF_DDOCS = 7.0f;
constexpr float DEF_UNOM1KTF = 35.0f;
constexpr float DEF_DUOSC = 2.0f;
constexpr float DEF_DIOSC = 2.0f;
constexpr float DEF_DUIMIN = 0.5f;
constexpr float DEF_IMIN = 0.2f;
constexpr DataTypes::DWORD DEF_T_DATA_RECKTF = 300.0f;
constexpr float DEF_U2NOM = 100.0f;
constexpr float DEF_ITT1NOM = 600.0f;
constexpr float DEF_ITT2NOM = 5.0f;
constexpr DataTypes::DWORD DEF_COOL_TYPE = 0.0f;
constexpr DataTypes::DWORD DEF_W_MAT = 0.0f;
constexpr float DEF_TNNTDOP = 105.0f;
constexpr float DEF_TNNTPRED = 100.0f;
constexpr float DEF_TAMB_NOM = 25.0f;
constexpr float DEF_DTNNT_NOM = 70.0f;
constexpr float DEF_KDOB = 0.1f;
constexpr float DEF_TAUWNOM = 10.0f;
constexpr float DEF_UMAX = 120.0f;
constexpr float DEF_IMAX = 33.0f;
constexpr float DEF_IWNOM = 300.0f;
constexpr DataTypes::DWORD DEF_OSCPOINTS = 32.0f;
constexpr float DEF_GTNNT = 2.0f;
constexpr float DEF_GOVC = 2.0f;
constexpr DataTypes::DWORD DEF_TDATNUM = 0.0f;
}

class ConfigKTF
{
public:
    explicit ConfigKTF();
    ~ConfigKTF();

    // AVM_KTF::Bci Bci_block;

    void SetDefConf();
    Config *MainConfig();
    ConfigKxx *KxxConfig();

private:
    Config *m_mainConfig;
    ConfigKxx *m_KxxConfig;

    //  S2DataTypes::S2ConfigType *m_S2Config;
};

#endif // CONFIGKTF_H
