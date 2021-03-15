#ifndef CONFIGKTF_H
#define CONFIGKTF_H

#include "../gen/datatypes.h"
#include "../module/modules.h"
#include "config.h"
#include "configkxx.h"

namespace AVM_KTF
{
constexpr unsigned DEF_NFILTR = 100;
constexpr unsigned DEF_NHARMFILT = 3;
constexpr unsigned DEF_DDOCS = 7;
constexpr unsigned DEF_UNOM1KTF = 35;
constexpr unsigned DEF_DUOSC = 2;
constexpr unsigned DEF_DIOSC = 2;
constexpr float DEF_DUIMIN = 0.5;
constexpr float DEF_IMIN = 0.2f;
constexpr unsigned DEF_T_DATA_RECKTF = 300;
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
}

class ConfigKTF
{
public:
    explicit ConfigKTF();
    ~ConfigKTF();

    AVM_KTF::Bci Bci_block;

    void SetDefConf();
    Config *MainConfig();
    ConfigKxx *KxxConfig();

private:
    Config *m_mainConfig;
    ConfigKxx *m_KxxConfig;

    S2DataTypes::S2ConfigType *m_S2Config;
};

#endif // CONFIGKTF_H
