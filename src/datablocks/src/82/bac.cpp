#include "datablocks/82/bac.h"

Bac82::Bac82(QObject *parent) : DataBlock(parent), m_blockData(std::make_unique<BlockData>())
{
    setBlock({ 2, "Настроечные параметры", DataTypes::DataBlockTypes::BacBlock, m_blockData.get(), sizeof(BlockData),
        true });
}

void Bac82::setupValuesDesc()
{
    addNewGroup("Коэффициенты по напряжению", "KmU", 0, 6, &m_blockData->KmU[0], 5);
    addNewGroup("Коэффициенты по току для Inom=5A", "KmI_5", 6, 6, &m_blockData->KmI_5[0], 5);
    addNewGroup("Коэффициенты по току для Inom=1A", "KmI_1", 12, 6, &m_blockData->KmI_1[0], 5);
    addNewGroup("Коррекция фазы", "DPsi", 18, 6, &m_blockData->DPsi[0], 5);
    addNewValue("K_Freq", "Коррекция частоты", &m_blockData->K_freq, 5);
    addNewValue("Kinter", "Коррекция взаимного влияния каналов", &m_blockData->Kinter, 5);
}

void Bac82::setDefBlock()
{
    setDefBlock(*m_blockData);
}

void Bac82::setDefBlock(Bac82::BlockData &blck)
{
    blck.Kinter = 0.0;
    blck.K_freq = 1.0;
    for (int i = 0; i < 6; i++)
    {
        blck.DPsi[i] = 0.0;
        blck.KmI_1[i] = 1.0;
        blck.KmI_5[i] = 1.0;
        blck.KmU[i] = 1.0;
    }
}

Bac82::BlockData *Bac82::data()
{
    return m_blockData.get();
}
