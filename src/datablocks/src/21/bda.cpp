#include "datablocks/21/bda.h"

Bda21::Bda21(u8 blockNum, QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ blockNum, "Сырые измерения", DataTypes::DataBlockTypes::BdaBlock, m_blockData.get(), sizeof(BlockData),
        false });
}

void Bda21::setupValuesDesc()
{
    addNewGroup("Данные без настройки", "bda", 8, 0, &m_blockData->sin[0], 5);
}

Bda21::BlockData *Bda21::data()
{
    return m_blockData.get();
}
