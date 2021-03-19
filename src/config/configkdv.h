#ifndef CONFIGKDV_H
#define CONFIGKDV_H

#include "config.h"
#include "configkxx.h"
namespace AVM_KDV
{
constexpr float DEF_UNOM1 = 0.38f;
constexpr float DEF_DUOSC = 2.0f;
constexpr float DEF_DIOSC = 2.0f;
constexpr float DEF_DUIMIN = 0.5f;
constexpr float DEF_IMIN = 0.2f;
constexpr float DEF_U2NOM = 100.0f;
constexpr float DEF_ITT1NOM = 600.0f;
constexpr float DEF_ITT2NOM = 5.0f;
constexpr float DEF_TNNTDOP = 105.0f;
constexpr float DEF_TNNTPRED = 100.0f;
constexpr float DEF_TAMB_NOM = 25.0f;
constexpr float DEF_DTNNT_NOM = 70.0f;
constexpr float DEF_KDOB = 0.1f;
constexpr float DEF_TAUWNOM = 10.0f;
constexpr float DEF_UMAX = 120.0f;
constexpr float DEF_IMAX = 33.0f;
constexpr float DEF_IWNOM = 300.0f;
constexpr float DEF_GTNNT = 2.0f;
constexpr float DEF_GOVC = 2.0f;

// constexpr DataTypes::DWORD DEF_EQ_TYPE = 0;
// constexpr DataTypes::DWORD DEF_COOL_TYPE = 0;
// constexpr DataTypes::DWORD DEF_W_MAT = 0;
constexpr float DEF_FNOM = 50.0f;
constexpr float DEF_NOM_SLIP = 0.03f;
constexpr float DEF_UVMAX = 5.0f;

constexpr float DEF_KVIBR = 0.1f;
constexpr float DEF_VIBRA_PRED = 8.0f;
constexpr float DEF_VIBRV_PRED = 25.0f;
constexpr float DEF_VIBRD_PRED = 80.0f;
constexpr float DEF_VIBRA_ALARM = 9.85f;
constexpr float DEF_VIBRV_ALARM = 31.4f;
constexpr float DEF_VIBRD_ALARM = 100.0f;
constexpr float DEF_VVIBRA_PRED = 0.3f;
constexpr float DEF_VVIBRV_PRED = 1.0f;
constexpr float DEF_VVIBRD_PRED = 3.0f;
constexpr float DEF_VVIBRA_ALARM = 0.6f;
constexpr float DEF_VVIBRV_ALARM = 2.0f;
constexpr float DEF_VVIBRD_ALARM = 6.0f;

constexpr DataTypes::DWORD DEF_DDOCS = 7;
constexpr DataTypes::DWORD DEF_NFILTR = 100;
constexpr DataTypes::DWORD DEF_NHARMFILT = 3;
constexpr DataTypes::DWORD DEF_NUMA = 1;
constexpr DataTypes::DWORD DEF_POLES = 2;
constexpr DataTypes::DWORD DEF_STATOR_SLOTES = 36;
constexpr DataTypes::DWORD DEF_ROTOR_BARS = 24;
constexpr DataTypes::DWORD DEF_VIBROTYPE = 0;
constexpr DataTypes::DWORD DEF_SENSORS = 0x03;
constexpr DataTypes::DWORD DEF_T_DATA_REC = 900;
constexpr DataTypes::DWORD DEF_OSCPOINTS = 32;
constexpr DataTypes::DWORD DEF_TDATNUM = 0;
}

class ConfigKDV
{
public:
    explicit ConfigKDV();
    ~ConfigKDV();

    // void setConfig();
    void SetDefConf();
    Config *MainConfig();
    ConfigKxx *KxxConfig();

private:
    Config *m_mainConfig;
    ConfigKxx *m_KxxConfig;
};

#endif // CONFIGKDV_H
