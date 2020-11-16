#ifndef BASECONFIG_H
#define BASECONFIG_H

#include "../gen/datablock.h"
#include "../gen/s2.h"

class BaseConfig
{
public:
    BaseConfig();
    QList<DataBlock *> dataBlockList();
    void addToBlockList(DataBlock *block);
    virtual void setConfig(S2ConfigType *s2config) = 0;

private:
    QList<DataBlock *> m_dataBlockList;
};

#endif // BASECONFIG_H
