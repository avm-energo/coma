#include "baseconfig.h"

BaseConfig::BaseConfig()
{
}

QList<DataBlock *> BaseConfig::dataBlockList()
{
    return m_dataBlockList;
}

void BaseConfig::addToBlockList(DataBlock *block)
{
    m_dataBlockList.append(block);
}
