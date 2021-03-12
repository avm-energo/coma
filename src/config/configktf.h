#ifndef CONFIGKTF_H
#define CONFIGKTF_H

#define DEF_NFILTR 100
#define DEF_NHARMFILT 3
#define DEF_DDOCS 7
#define DEF_UNOM1KTF 35
#define DEF_DUOSC 2
#define DEF_DIOSC 2
#define DEF_DUIMIN 0.5
#define DEF_IMIN 0.2
#define DEF_T_DATA_RECKTF 300
#define DEF_U2NOM 100
#define DEF_ITT1NOM 600
#define DEF_ITT2NOM 5
#define DEF_COOL_TYPE 0
#define DEF_W_MAT 0
#define DEF_TNNTDOP 105
#define DEF_TNNTPRED 100
#define DEF_TAMB_NOM 25
#define DEF_DTNNT_NOM 70
#define DEF_KDOB 0.1
#define DEF_TAUWNOM 10
#define DEF_UMAX 120
#define DEF_IMAX 33
#define DEF_IWNOM 300
#define DEF_OSCPOINTS 32
#define DEF_GTNNT 2
#define DEF_GOVC 2
#define DEF_TDATNUM 0

#include "../gen/datatypes.h"
#include "../module/modules.h"
#include "config.h"
#include "configkxx.h"

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
