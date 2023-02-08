#include "bdain.h"

#include "../../widgets/wd_func.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <gen/colors.h>
#include <gen/stdfunc.h>

BdaIn::BdaIn(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 1, "Текущие данные", DataTypes::DataBlockTypes::BdBlock, m_blockData.get(), sizeof(BlockData) });
}

void BdaIn::setupValuesDesc()
{
    addNewGroup("Истинные действующие значения напряжения в В на входе", "IUefNat_filt[U]", 3, 0,
        &m_blockData->IUefNat_filt[0], 3);
    addNewGroup(
        "Истинные действующие значения тока в мА на входе", "IUefNat_filt[I]", 3, 3, &m_blockData->IUefNat_filt[3], 4);
    addNewGroup(
        "Действующие значения сигналов по 1-й гармонике", "IUeff_filtered", 6, 6, &m_blockData->IUeff_filtered[0], 3);
    addNewGroup("Углы сдвига сигналов по 1-й гармонике относительно Ua в градусах", "phi_next_f", 12, 6,
        &m_blockData->phi_next_f[0], 4);
    addNewGroup("Емкости вводов", "Cbush", 18, 3, &m_blockData->Cbush[0], 1);
    addNewGroup("Tg delta вводов", "Tg d", 21, 3, &m_blockData->Tg_d[0], 4);

    addNewValue("Frequency", "Частота сигналов, Гц", &m_blockData->Frequency, 4);

    addNewValue("Pt100", "Измеренное сопротивление термометра, Ом", &m_blockData->Pt100_R, 3);
}

BdaIn::BlockData *BdaIn::data()
{
    return m_blockData.get();
}
