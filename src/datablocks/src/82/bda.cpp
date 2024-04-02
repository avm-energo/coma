#include "datablocks/82/bda.h"

/// TODO: [Card #28627204]
#include <widgets/wd_func.h>

Bda82::Bda82(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 1, "Текущие данные", DataTypes::DataBlockTypes::BdaBlock, m_blockData.get(), sizeof(BlockData), false });
    createWidget();
}

void Bda82::setupValuesDesc()
{
    addNewValue("Частота:", "Частота сигналов, Гц", &m_blockData->Frequency, 3);
    addNewGroup("Значения сигналов в единицах АЦП", "NADC", 0, 6, &m_blockData->IUeff_ADC[0], 0);
}

Error::Msg Bda82::checkValues(Device::MezzanineBoard mtypeM, S2::FLOAT_6t i2Noms)
{
    // we suggest that each three of currents are equal inside each other
    assert(i2Noms.size() > 3);
    float iNom = (i2Noms.at(0) == 5) ? 419500.0 : 1678000.0;
    float iTol = (i2Noms.at(3) == 5) ? 21000.0 : 84000.0;
    float uNom = 2088500.0;
    float uTol = 105000.0;

    Error::Msg res;
    switch (mtypeM)
    {
    case Device::MezzanineBoard::MTM_81: // 2t0n
        res = checkAnalogValues(
            { iNom, iNom, iNom, iNom, iNom, iNom, 50.0 }, { iTol, iTol, iTol, iTol, iTol, iTol, 0.05 });
        break;
    case Device::MezzanineBoard::MTM_82:
        res = checkAnalogValues(
            { uNom, uNom, uNom, iNom, iNom, iNom, 50.0 }, { uTol, uTol, uTol, iTol, iTol, iTol, 0.05 });
        break;
    case Device::MezzanineBoard::MTM_83: // 0t2n
        res = checkAnalogValues(
            { uNom, uNom, uNom, uNom, uNom, uNom, 50.0 }, { uTol, uTol, uTol, uTol, uTol, uTol, 0.05 });
        break;
    default:
        return Error::Msg::GeneralError;
    }
    return res;
}

Error::Msg Bda82::checkAnalogValues(QList<float> valuesToCheck, QList<float> tolerances)
{
    const QStringList valueNames
        = { "канала №1", "канала №2", "канала №3", "канала №4", "канала №5", "канала №6", "частоты" };
    assert(valuesToCheck.size() == valueNames.size());
    assert(valuesToCheck.size() == tolerances.size());
    updateFromWidget();
    BlockData *bd = data();
    for (int i = 0; i < valuesToCheck.size(); ++i)
    {
        float tmpf = *(reinterpret_cast<float *>(bd) + i);
        if (!WDFunc::floatIsWithinLimits(valueNames.at(i), tmpf, valuesToCheck.at(i), tolerances.at(i)))
            return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}

Bda82::BlockData *Bda82::data()
{
    return m_blockData.get();
}
