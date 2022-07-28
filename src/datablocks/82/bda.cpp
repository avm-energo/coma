#include "bda.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/wd_func.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QtMath>

Bda82::Bda82(Modules::MezzanineBoard typem, QObject *parent) : DataBlock(parent)
{
    m_typeM = typem;
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 0, "Текущие данные", DataTypes::DataBlockTypes::BdaBlock, m_blockData.get(), sizeof(BlockData) });
}

void Bda82::createWidget()
{
    m_widget = new QWidget;
    int i;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(m_widget, "Частота:"));
    hlyout->addWidget(WDFunc::NewLE2(m_widget, "value0", "Частота сигналов, Гц"), Qt::AlignLeft);
    lyout->addLayout(hlyout);
    for (i = 1; i < 7; ++i)
    {
        QString IndexStr = "[" + QString::number(i - 1) + "]";
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "IUNF" + IndexStr), 0, (i - 1), 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "value" + QString::number(i),
                              QString::number(i) + "IUNF" + IndexStr + ".Истинные действующие значения сигналов"),
            1, (i - 1), 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "IUF" + IndexStr), 2, (i - 1), 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "value" + QString::number(i + 6),
                              QString::number(i + 6) + "IUF" + IndexStr
                                  + ".Действующие значения сигналов по 1-й гармонике\nотносительно ф. А 1-й группы"),
            3, (i - 1), 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "PHF" + IndexStr), 4, (i - 1), 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "value" + QString::number(i + 12),
                              QString::number(i + 12) + "PHF" + IndexStr
                                  + ".Угол сдвига между сигналами по первой гармонике\nотносительно ф. А 1-й группы"),
            5, (i - 1), 1, 1);
    }
    for (i = 0; i < 3; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "PNF" + IndexStr), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "value" + QString::number(i + 19),
                              QString::number(i + 19) + ".Истинная активная мощность"),
            7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "SNF" + IndexStr), 6, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "value" + QString::number(i + 22),
                              QString::number(i + 22) + ".Кажущаяся полная мощность"),
            7, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "QNF" + IndexStr), 8, i, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "value" + QString::number(i + 25),
                              QString::number(i + 25) + ".Реактивная мощность"),
            9, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "Cos" + IndexStr), 8, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "value" + QString::number(i + 28),
                              QString::number(i + 28) + ".Cos phi по истинной активной мощности"),
            9, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "PF" + IndexStr), 10, i, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "value" + QString::number(i + 31),
                              QString::number(i + 31) + ".Активная мощность по 1-й гармонике"),
            11, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "QF" + IndexStr), 10, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "value" + QString::number(i + 34),
                              QString::number(i + 34) + ".Реактивная мощность по 1-й гармонике"),
            11, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "SF" + IndexStr), 12, i, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "value" + QString::number(i + 37),
                              QString::number(i + 37) + ".Полная мощность по 1-й гармонике"),
            13, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "CosPhi" + IndexStr), 12, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "value" + QString::number(i + 40),
                              QString::number(i + 40) + ".Cos phi по 1-й гармонике"),
            13, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "PHI" + IndexStr), 14, i, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "value" + QString::number(i + 43),
                              QString::number(i + 43) + ".Угол между током и напряжением"),
            15, i, 1, 1);
    }
    lyout->addLayout(glyout);
    lyout->addStretch(100);
    m_widget->setLayout(lyout);
    m_widgetIsSet = true;
}

void Bda82::setDefBlock()
{
}

void Bda82::updateWidget()
{
    WDFunc::SetLBLText(m_widget, "value0", WDFunc::StringValueWithCheck(m_blockData->Frequency, 3));
    for (int i = 1; i < 4; i++)
    {
        int Precision = (m_typeM != Modules::MezzanineBoard::MTM_81) ? 3 : 4;
        WDFunc::SetLBLText(m_widget, "value" + QString::number(i),
            WDFunc::StringValueWithCheck(m_blockData->IUefNat_filt[i - 1], Precision));
        WDFunc::SetLBLText(m_widget, "value" + QString::number(i + 6),
            WDFunc::StringValueWithCheck(m_blockData->IUeff_filtered[i - 1], Precision));
        Precision = (m_typeM != Modules::MezzanineBoard::MTM_83) ? 4 : 3;
        WDFunc::SetLBLText(m_widget, "value" + QString::number(i + 3),
            WDFunc::StringValueWithCheck(m_blockData->IUefNat_filt[i + 2], Precision));
        WDFunc::SetLBLText(m_widget, "value" + QString::number(i + 9),
            WDFunc::StringValueWithCheck(m_blockData->IUeff_filtered[i + 2], Precision));
    }
    for (int i = 1; i < 7; i++)
    {
        WDFunc::SetLBLText(m_widget, "value" + QString::number(i + 12),
            WDFunc::StringValueWithCheck(m_blockData->phi_next_f[i - 1], 4));
    }

    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLBLText(
            m_widget, "value" + QString::number(i + 19), WDFunc::StringValueWithCheck(m_blockData->PNatf[i], 3));
        WDFunc::SetLBLText(
            m_widget, "value" + QString::number(i + 22), WDFunc::StringValueWithCheck(m_blockData->SNatf[i], 3));
        WDFunc::SetLBLText(
            m_widget, "value" + QString::number(i + 25), WDFunc::StringValueWithCheck(m_blockData->QNatf[i], 3));
        WDFunc::SetLBLText(
            m_widget, "value" + QString::number(i + 28), WDFunc::StringValueWithCheck(m_blockData->CosPhiNat[i], 4));
        WDFunc::SetLBLText(
            m_widget, "value" + QString::number(i + 31), WDFunc::StringValueWithCheck(m_blockData->Pf[i], 3));
        WDFunc::SetLBLText(
            m_widget, "value" + QString::number(i + 34), WDFunc::StringValueWithCheck(m_blockData->Qf[i], 3));
        WDFunc::SetLBLText(
            m_widget, "value" + QString::number(i + 37), WDFunc::StringValueWithCheck(m_blockData->Sf[i], 3));
        WDFunc::SetLBLText(
            m_widget, "value" + QString::number(i + 40), WDFunc::StringValueWithCheck(m_blockData->CosPhi[i], 4));
        float PHI = (180 * qAsin(m_blockData->Qf[i] / m_blockData->Sf[i]) / M_PI);
        WDFunc::SetLBLText(m_widget, "value" + QString::number(i + 43), WDFunc::StringValueWithCheck(PHI, 4));
    }
}

Error::Msg Bda82::checkValues(float voltage, float current, float degree, float tolerance)
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

Error::Msg Bda82::checkAnalogValues(double u, double i, double p, double q, double s, double phi, double cosphi,
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
    for (int i = 0; i < 43; i++)
    {
        float tmpf = WDFunc::LEData(m_widget, "value" + QString::number(i)).toFloat();
        if (!WDFunc::floatIsWithinLimits(valueNames.at(i), tmpf, ValuesToCheck.at(i), ThresholdsToCheck.at(i)))
            return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}

Bda82::BlockData *Bda82::data()
{
    return m_blockData.get();
}
