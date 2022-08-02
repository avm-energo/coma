#include "bda.h"

#include <QGroupBox>
#include <QVBoxLayout>

Bda::Bda(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 0, "Текущие данные", DataTypes::DataBlockTypes::BdaBlock, m_blockData.get(), sizeof(BlockData) });
    createWidget();
}

void Bda::setupValuesDesc()
{
    m_valuesDesc.append(addGroupToValues(
        "Измеренные сигналы в кодах АЦП без регулировки", "Ueff_ADC", 0, 6, &m_blockData->Ueff_ADC[0], 4));
    m_valuesDesc.append({ "", { { "Frequency", "Частота, Гц", "bdavalue6", &m_blockData->Frequency, 4 } } });
    m_valuesDesc.append({ "", { { "Pt100", "Температура, Ом", "bdavalue7", &m_blockData->Pt100, 4 } } });
}

Bda::BlockData *Bda::data()
{
    return m_blockData.get();
}
