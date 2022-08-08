#include "bac.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/wd_func.h"

#include <QGroupBox>
#include <QVBoxLayout>

Bac82::Bac82(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 2, "Настроечные параметры", DataTypes::DataBlockTypes::BacBlock, m_blockData.get(), sizeof(BlockData) });
    readBlockFromModule();
}

void Bac82::setupValuesDesc()
{
    addNewGroup("Коэффициенты по напряжению", "KmU", 6, 0, &m_blockData->KmU[0], 5);
    addNewGroup("Коэффициенты по току для Inom=5A", "KmI_5", 6, 6, &m_blockData->KmI_5[0], 5);
    addNewGroup("Коэффициенты по току для Inom=1A", "KmI_1", 6, 12, &m_blockData->KmI_1[0], 5);
    addNewGroup("Коррекция фазы", "DPsi", 6, 18, &m_blockData->DPsi[0], 5);
    addNewValue("K_Freq", "Коррекция частоты", &m_blockData->K_freq, 5);
    addNewValue("Kinter", "Коррекция взаимного влияния каналов", &m_blockData->Kinter, 5);
}

void Bac82::setDefBlock()
{
    m_blockData->Kinter = 0.0;
    m_blockData->K_freq = 1.0;
    for (int i = 0; i < 6; i++)
    {
        m_blockData->DPsi[i] = 0.0;
        m_blockData->KmI_1[i] = 1.0;
        m_blockData->KmI_5[i] = 1.0;
        m_blockData->KmU[i] = 1.0;
    }
    writeBlockToModule();
}

Bac82::BlockData *Bac82::data()
{
    return m_blockData.get();
}
