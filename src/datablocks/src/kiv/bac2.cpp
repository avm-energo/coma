#include "datablocks/kiv/bac2.h"

Bac2A284::Bac2A284(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 1, "Настроечные параметры", DataTypes::DataBlockTypes::BacBlock, m_blockData.get(), sizeof(BlockData),
        true });
}

void Bac2A284::setupValuesDesc()
{
    addNewValue("K_freq", "Коррекция частоты", &m_blockData->K_freq);
    addNewValue("Art", "Коэффициент в канале Pt100", &m_blockData->Art);
    addNewValue("Brt", "Смещение в канале Pt100", &m_blockData->Brt);
}

void Bac2A284::setDefBlock()
{
    m_blockData->Art = 81.5f;
    m_blockData->Brt = 6033.0f;
    m_blockData->K_freq = 1;
}

Bac2A284::BlockData *Bac2A284::data()
{
    return m_blockData.get();
}
