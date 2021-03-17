#ifndef CONFIGKDV_H
#define CONFIGKDV_H

#include "../gen/datatypes.h"
#include "../module/modules.h"
#include "config.h"
#include "configkxx.h"

namespace AVM_KDV
{
constexpr unsigned DEF_NFILTR = 100;
constexpr unsigned DEF_NHARMFILT = 3;
constexpr unsigned DEF_DDOCS = 7;
constexpr float DEF_UNOM1 = 0.38f;
constexpr unsigned DEF_DUOSC = 2;
constexpr unsigned DEF_DIOSC = 2;
constexpr float DEF_DUIMIN = 0.5;
constexpr float DEF_IMIN = 0.2f;
constexpr unsigned DEF_T_DATA_REC = 900;
constexpr unsigned DEF_U2NOM = 100;
constexpr unsigned DEF_ITT1NOM = 600;
constexpr unsigned DEF_ITT2NOM = 5;
constexpr unsigned DEF_COOL_TYPE = 0;
constexpr unsigned DEF_W_MAT = 0;
constexpr unsigned DEF_TNNTDOP = 105;
constexpr unsigned DEF_TNNTPRED = 100;
constexpr unsigned DEF_TAMB_NOM = 25;
constexpr unsigned DEF_DTNNT_NOM = 70;
constexpr float DEF_KDOB = 0.1f;
constexpr unsigned DEF_TAUWNOM = 10;
constexpr unsigned DEF_UMAX = 120;
constexpr unsigned DEF_IMAX = 33;
constexpr unsigned DEF_IWNOM = 300;
constexpr unsigned DEF_OSCPOINTS = 32;
constexpr unsigned DEF_GTNNT = 2;
constexpr unsigned DEF_GOVC = 2;
constexpr unsigned DEF_TDATNUM = 0;
constexpr unsigned DEF_EQ_TYPE = 0;
constexpr unsigned DEF_FNOM = 50;
constexpr float DEF_NOM_SLIP = 0.03f;
constexpr float DEF_UVMAX = 5.0f;
constexpr unsigned DEF_POLES = 2;
constexpr unsigned DEF_STATOR_SLOTES = 36;
constexpr unsigned DEF_ROTOR_BARS = 24;
constexpr unsigned DEF_VIBROTYPE = 0;
constexpr unsigned DEF_SENSORS = 0x03;
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
constexpr unsigned DEF_NUMA = 1;
}

class ConfigKDV
{
public:
    explicit ConfigKDV();

    AVM_KDV::Bci Bci_block;
    void setConfig();
    void SetDefConf();
    Config *MainConfig();
    ConfigKxx *KxxConfig();

private:
    Config *m_mainConfig;
    ConfigKxx *m_KxxConfig;

    S2DataTypes::S2ConfigType *m_S2Config;
};

#endif // CONFIGKDV_H
