#include "datablocks/84/bac.h"

Bac84::Bac84(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 1, "Настроечные параметры", DataTypes::DataBlockTypes::BacBlock, m_blockData.get(), sizeof(BlockData),
        true });
}

void Bac84::setupValuesDesc()
{
    addNewValue("K_freq", "Коррекция частоты", &m_blockData->K_freq);
}

void Bac84::setDefBlock()
{
    m_blockData->K_freq = 1;
}

Bac84::BlockData *Bac84::data()
{
    return m_blockData.get();
}
