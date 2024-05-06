#include "datablocks/21/bda.h"

Bda21::Bda21(QObject *parent) : DataBlock(parent)
{
    m_blockData = new BlockData;
    setBlock({ 1, "Сырые измерения", DataTypes::DataBlockTypes::BdaBlock, m_blockData, sizeof(BlockData), false });
}

void Bda21::setupValuesDesc()
{
    addNewGroup("Данные без настройки", "bda", 8, 0, &m_blockData->sin[0], 5);
}
