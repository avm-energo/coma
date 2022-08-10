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
    addNewValue("Tmk0:", "Температура процессора, C", &m_blockData->Tmk, 3);
    addNewValue("VBat:", "Напряжение батарейки, В", &m_blockData->Vbat, 3);
}
