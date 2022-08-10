#include "bda.h"

#include <QGroupBox>
#include <QVBoxLayout>

BdaA284::BdaA284(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 0, "Текущие данные", DataTypes::DataBlockTypes::BdaBlock, m_blockData.get(), sizeof(BlockData) });
    createWidget();
}

void BdaA284::setupValuesDesc()
{
    addNewGroup("Измеренные сигналы в кодах АЦП без регулировки", "Ueff_ADC", 0, 6, &m_blockData->Ueff_ADC[0], 4);
    addNewValue("Frequency", "Частота, Гц", &m_blockData->Frequency, 4);
    addNewValue("Pt100", "Температура, Ом", &m_blockData->Pt100, 4);
}

BdaA284::BlockData *BdaA284::data()
{
    return m_blockData.get();
}
