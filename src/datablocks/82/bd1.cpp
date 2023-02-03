#include "bd1.h"

#include "../../widgets/wd_func.h"

#include <QtMath>
#include <gen/stdfunc.h>

Bd182::Bd182(Modules::MezzanineBoard typem, QObject *parent) : DataBlock(parent)
{
    m_typeM = typem;
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 1, "Текущие данные", DataTypes::DataBlockTypes::BdaBlock, m_blockData.get(), sizeof(BlockData), false });
}

void Bd182::setupValuesDesc()
{
    int precision = (m_typeM != Modules::MezzanineBoard::MTM_81) ? 3 : 4;
    addNewValue("Частота:", "Частота сигналов, Гц", &m_blockData->Frequency, 3);
    addNewGroup("Истинные действующие значения сигналов", "IUNF", 6, 0, &m_blockData->IUefNat_filt[0], precision);
    addNewGroup("Действующие значения сигналов по 1-й гармонике\nотносительно ф. А 1-й группы", "IUF", 6, 6,
        &m_blockData->IUeff_filtered[0], precision);
    addNewGroup("Угол сдвига между сигналами по первой гармонике\nотносительно ф. А 1-й группы", "PHF", 6, 12,
        &m_blockData->phi_next_f[0], 4);
    addNewGroup("Истинная активная мощность", "PNF", 3, 18, &m_blockData->PNatf[0], 3);
    addNewGroup("Кажущаяся полная мощность", "SNF", 3, 21, &m_blockData->SNatf[0], 3);
    addNewGroup("Реактивная мощность", "QNF", 3, 24, &m_blockData->QNatf[0], 3);
    addNewGroup("Cos phi по истинной активной мощности", "Cos", 3, 27, &m_blockData->CosPhiNat[0], 4);
    addNewGroup("Активная мощность по 1-й гармонике", "PF", 3, 30, &m_blockData->Pf[0], 3);
    addNewGroup("Полная мощность по 1-й гармонике", "SF", 3, 33, &m_blockData->Sf[0], 3);
    addNewGroup("Реактивная мощность по 1-й гармонике", "QF", 3, 36, &m_blockData->Qf[0], 3);
    addNewGroup("Cos phi по 1-й гармонике", "CosPhi", 3, 39, &m_blockData->CosPhi[0], 4);
    addNewGroup("Угол между током и напряжением", "PHI", 3, 42, &PHI[0], 4);
}

void Bd182::specificUpdateWidget()
{
    for (int i = 0; i < 3; ++i)
        PHI[i] = (180 * qAsin(m_blockData->Qf[i] / m_blockData->Sf[i]) / M_PI);
}

Error::Msg Bd182::checkValues(float voltage, float current, float degree, float tolerance)
{
    double phi = qDegreesToRadians(degree);
    double p = voltage * current * qCos(phi);
    double q = voltage * current * qSin(phi);
    double s = qSqrt(p * p + q * q);
    double utol, itol, ptol, dtol, ctol;
    if (tolerance) // точные погрешности
    {
        utol = 0.1;
        itol = 0.005;
        ptol = 0.1;
        dtol = 0.1;
        ctol = 0.005;
    }
    else
    {
        utol = 0.5;
        itol = 0.05;
        ptol = 0.5;
        dtol = 1;
        ctol = 0.05;
    }
    Error::Msg res;
    switch (m_typeM)
    {
    case Modules::MezzanineBoard::MTM_81: // 2t0n
        res = checkAnalogValues(current, current, 0.0, 0.0, 0.0, 0.0, 0.0, utol, itol, ptol, dtol, ctol);
        break;
    case Modules::MezzanineBoard::MTM_82:
        res = checkAnalogValues(voltage, current, p, q, s, phi, qCos(phi), utol, itol, ptol, dtol, ctol);
        break;
    case Modules::MezzanineBoard::MTM_83: // 0t2n
        res = checkAnalogValues(voltage, voltage, 0.0, 0.0, 0.0, 0.0, 0.0, utol, itol, ptol, dtol, ctol);
        break;
    default:
        return Error::Msg::GeneralError;
    }
    return res;
}

Error::Msg Bd182::checkAnalogValues(double u, double i, double p, double q, double s, double phi, double cosphi,
    double utol, double itol, double pht, double pt, double ct)
{
    double it = (m_typeM == Modules::MezzanineBoard::MTM_83) ? utol : itol; // 0t2n
    double ut = (m_typeM == Modules::MezzanineBoard::MTM_81) ? itol : utol; // 2t0n
    const QStringList valueNames = { "частоты", "напряжения ф. А", "напряжения ф. В", "напряжения ф. С", "тока ф. А",
        "тока ф. В", "тока ф. С", "напряжения ф. А", "напряжения ф. В", "напряжения ф. С", "тока ф. А", "тока ф. В",
        "тока ф. С", "угла uA, uA", "угла uA, uB", "угла uA, uC", "угла uA, iA", "угла uA, iB", "угла uA, iC",
        "акт. мощности ф. А", "акт. мощности ф. В", "акт. мощности ф. С", "полн. мощности ф. А", "полн. мощности ф. В",
        "полн. мощности ф. С", "реакт. мощности ф. А", "реакт. мощности ф. В", "реакт. мощности ф. С", "cos ф. А",
        "cos ф. В", "cos ф. С", "акт. мощности по 1 гарм ф. А", "акт. мощности по 1 гарм ф. В",
        "акт. мощности по 1 гарм ф. С", "полн. мощности по 1 гарм ф. А", "полн. мощности по 1 гарм ф. В",
        "полн. мощности по 1 гарм ф. С", "реакт. мощности по 1 гарм ф. А", "реакт. мощности по 1 гарм ф. В",
        "реакт. мощности по 1 гарм ф. С", "cos по 1 гарм ф. А", "cos по 1 гарм ф. В", "cos по 1 гарм ф. С" };
    const QList<double> ValuesToCheck = { 50.0, u, u, u, i, i, i, u, u, u, i, i, i, phi, phi, phi, phi, phi, phi, p, p,
        p, s, s, s, q, q, q, cosphi, cosphi, cosphi, p, p, p, q, q, q, s, s, s, cosphi, cosphi, cosphi };
    const QList<double> ThresholdsToCheck = { 0.005, ut, ut, ut, it, it, it, ut, ut, ut, it, it, it, pht, pht, pht, pht,
        pht, pht, pt, pt, pt, pt, pt, pt, pt, pt, pt, ct, ct, ct, pt, pt, pt, pt, pt, pt, pt, pt, pt, ct, ct, ct };
    updateFromWidget();
    BlockData *bd = data();
    for (int i = 0; i < 43; i++)
    {
        float tmpf = *(reinterpret_cast<float *>(bd) + i);
        if (!WDFunc::floatIsWithinLimits(valueNames.at(i), tmpf, ValuesToCheck.at(i), ThresholdsToCheck.at(i)))
            return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}

Bd182::BlockData *Bd182::data()
{
    return m_blockData.get();
}
