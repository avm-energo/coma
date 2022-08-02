#include "bd0.h"

Bd0::Bd0(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 0, "Общие", DataTypes::DataBlockTypes::BdBlock, m_blockData.get(), sizeof(BlockData) });
}

Bd0::BlockData *Bd0::data()
{
    return m_blockData.get();
}

void Bd0::setupValuesDesc()
{
    m_valuesDesc.append({ "", { { "Tmk0:", "Температура процессора, C", "value0", &m_blockData->Tmk, 3 } } });
    m_valuesDesc.append({ "", { { "VBat:", "Напряжение батарейки, В", "value1", &m_blockData->Vbat, 3 } } });
}
