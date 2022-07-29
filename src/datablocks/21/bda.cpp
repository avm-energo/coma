#include "bda.h"

Bda21::Bda21(QObject *parent) : DataBlock(parent)
{
    m_blockData = new BlockData;
    setBlock({ 1, "Сырые измерения", DataTypes::DataBlockTypes::BdaBlock, m_blockData, sizeof(BlockData) });
}

void Bda21::setupValuesDesc()
{
    m_valuesDesc.append(addGroupToValues("Данные без настройки", "bda", 8, 0, &m_blockData->sin[0], 5));
}
