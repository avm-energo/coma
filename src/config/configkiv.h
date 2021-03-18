#ifndef CONFIGKIV_H
#define CONFIGKIV_H

#include "config.h"
#include "configkxx.h"

class ConfigKIV
{
public:
    explicit ConfigKIV();
    ~ConfigKIV();

    void setDefConf();
    Config *MainConfig();
    ConfigKxx *KxxConfig();

private:
    Config *m_mainConfig;
    ConfigKxx *m_KxxConfig;
};

#endif // CONFIGKIV_H
