#include "datablocks/21/bda.h"

Bda21::Bda21(u8 blockNum, QObject *parent) : DataBlock(parent)
{
    m_blockData = new BlockData;
    setBlock(
        { blockNum, "Сырые измерения", DataTypes::DataBlockTypes::BdaBlock, m_blockData, sizeof(BlockData), false });
}

void Bda21::setupValuesDesc()
{
    addNewGroup("Данные без настройки", "bda", 0, 8, &m_blockData->sin[0], 5);
}

Bda21::BlockData *Bda21::data()
{
    return m_blockData;
}
