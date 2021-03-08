#ifndef CONFIGKIV_H
#define CONFIGKIV_H

#include "../module/module_kiv.h"
#include "config.h"
#include "configkxx.h"

class ConfigKIV
{
public:
    explicit ConfigKIV();
    ~ConfigKIV();

    AVM_KIV::Bci Bci_block;

    void setDefConf();
    Config *MainConfig();
    ConfigKxx *KxxConfig();

private:
    Config *m_mainConfig;
    ConfigKxx *m_KxxConfig;

    S2DataTypes::S2ConfigType *m_S2Config;
};

#endif // CONFIGKIV_H
