#include "datablocks/84/bda.h"

Bda84::Bda84(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 1, "Текущие данные", DataTypes::DataBlockTypes::BdaBlock, m_blockData.get(), sizeof(BlockData), false });
    createGeneralWidget();
}

void Bda84::setupValuesDesc()
{
    addNewGroup("Измеренные сигналы в кодах АЦП без регулировки", "Ueff_ADC", 0, 6, &m_blockData->Ueff_ADC[0], 0);
    addNewValue("Frequency", "Частота, Гц", &m_blockData->Frequency, 4);
    addNewValue("Pt100", "Температура, Ом", &m_blockData->Pt100, 4);
}

Bda84::BlockData *Bda84::data()
{
    return m_blockData.get();
}
