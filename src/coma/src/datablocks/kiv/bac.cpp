#include "datablocks/kiv/bac.h"

BacA284::BacA284(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 1, "Настроечные параметры", DataTypes::DataBlockTypes::BacBlock, m_blockData.get(), sizeof(BlockData),
        true });
}

void BacA284::setupValuesDesc()
{
    addNewValue("K_freq", "Коррекция частоты", &m_blockData->K_freq);
    addNewValue("Art", "Коэффициент в канале Pt100", &m_blockData->Art);
    addNewValue("Brt", "Смещение в канале Pt100", &m_blockData->Brt);
}

void BacA284::setDefBlock()
{
    m_blockData->Art = 81.5f;
    m_blockData->Brt = 6033.0f;
    m_blockData->K_freq = 1;
}

BacA284::BlockData *BacA284::data()
{
    return m_blockData.get();
}
