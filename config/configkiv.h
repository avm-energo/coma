#ifndef CONFIGKIV_H
#define CONFIGKIV_H

#include "../module/module_kiv.h"
#include "config.h"
#include "configkxx.h"

class ConfigKIV
{
public:
    explicit ConfigKIV(S2DataTypes::S2ConfigType *config);

    AVM_KIV::Bci Bci_block;

    //    QStack<Bci> m_BciStack;

    void setDefConf();
    //    Error::Msg getAndPushConfig();
    //    Bci popConfig();
    //    Error::Msg popAndWriteConfig();
    S2DataTypes::S2ConfigType *S2Config();
    void setS2Config(S2DataTypes::S2ConfigType *s2config);
    Config *MainConfig();
    ConfigKxx *KxxConfig();

private:
    Config *m_mainConfig;
    ConfigKxx *m_KxxConfig;

    S2DataTypes::S2ConfigType *m_S2Config;
};

#endif // CONFIGKIV_H
