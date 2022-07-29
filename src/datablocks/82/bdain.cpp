#include "bdain.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/wd_func.h"

#include <QGroupBox>
#include <QVBoxLayout>

BdaIn82::BdaIn82(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 1, "Текущие данные", DataTypes::DataBlockTypes::BdBlock, m_blockData.get(), sizeof(BlockData) });
}

void BdaIn82::setupValuesDesc()
{
    m_valuesDesc.append(addGroupToValues("Истинные действующие значения напряжения в В на входе", "IUefNat_filt[U]", 3,
        0, &m_blockData->IUefNat_filt[0], 3));
    m_valuesDesc.append(addGroupToValues(
        "Истинные действующие значения тока в мА на входе", "IUefNat_filt[I]", 3, 3, &m_blockData->IUefNat_filt[3], 4));
    m_valuesDesc.append(addGroupToValues(
        "Действующие значения сигналов по 1-й гармонике", "IUeff_filtered", 6, 6, &m_blockData->IUeff_filtered[0], 3));
    m_valuesDesc.append(addGroupToValues("Углы сдвига сигналов по 1-й гармонике относительно Ua в градусах",
        "phi_next_f", 12, 6, &m_blockData->phi_next_f[0], 4));

    m_valuesDesc.append(addGroupToValues("Емкости вводов", "Cbush", 18, 3, &m_blockData->Cbush[0], 1));
    m_valuesDesc.append(addGroupToValues("Tg delta вводов", "Tg d", 21, 3, &m_blockData->Tg_d[0], 4));
    m_valuesDesc.append(
        { "", { { "Frequency", "Частота сигналов, Гц", "bdainvalue24", &m_blockData->Frequency, 4 } } });
    m_valuesDesc.append(
        { "", { { "Pt100", "Измеренное сопротивление термометра, Ом", "", &m_blockData->Pt100_R, 3 } } });
}

void BdaIn82::createWidget()
{
    m_widget = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    //    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Измеренные данные");
    for (int i = 0; i < 3; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "IUefNat_filt" + IndexStr), 0, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "bdainvalue" + QString::number(i), ""), 1, i, 1, 1);
    }
    for (int i = 3; i < 6; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "IUefNat_filt" + IndexStr), 0, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "bdainvalue" + QString::number(i),
                              "Истинные действующие значения тока в мА на входе"),
            1, i, 1, 1);
    }
    for (int i = 0; i < 6; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";

        glyout->addWidget(WDFunc::NewLBL2(m_widget, "IUeff_filtered" + IndexStr), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "bdainvalue" + QString::number(i + 6),
                              "Действующие значения сигналов по 1-й гармонике"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "phi_next_f" + IndexStr), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "bdainvalue" + QString::number(i + 12),
                              "Углы сдвига сигналов по 1-й гармонике относительно Ua в градусах"),
            5, i, 1, 1);
    }
    for (int i = 0; i < 3; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "Cbush" + IndexStr), 6, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT2(m_widget, "", "bdainvalue" + QString::number(i + 18), "Емкости вводов"), 7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "Tg d" + IndexStr), 6, i + 3, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT2(m_widget, "", "bdainvalue" + QString::number(i + 21), "Tg delta вводов"), 7, i + 3, 1, 1);
    }
    glyout->addWidget(WDFunc::NewLBL2(m_widget, "Frequency"), 8, 0, 1, 3);
    glyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "bdainvalue24", "Частота в сети, Гц"), 9, 0, 1, 3);
    glyout->addWidget(WDFunc::NewLBL2(m_widget, "Pt100"), 8, 3, 1, 3);
    glyout->addWidget(
        WDFunc::NewLBLT2(m_widget, "", "bdainvalue25", "Измеренное сопротивление термометра, Ом"), 9, 3, 1, 3);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    m_widget->setLayout(lyout);
    m_widgetIsSet = true;
}

void BdaIn82::updateWidget()
{
    if (m_widgetIsSet)
    {
        for (int i = 0; i < 6; i++)
        {
            WDFunc::SetLBLText(m_widget, "bdainvalue" + QString::number(i),
                WDFunc::StringFloatValueWithCheck(m_blockData->IUefNat_filt[i], 3));
            WDFunc::SetLBLText(m_widget, "bdainvalue" + QString::number(i + 6),
                WDFunc::StringFloatValueWithCheck(m_blockData->IUeff_filtered[i], 3));
            WDFunc::SetLBLText(m_widget, "bdainvalue" + QString::number(i + 12),
                WDFunc::StringFloatValueWithCheck(m_blockData->phi_next_f[i], 4));
            if (i < 3)
            {
                WDFunc::SetLBLText(m_widget, "bdainvalue" + QString::number(i + 18),
                    WDFunc::StringFloatValueWithCheck(m_blockData->Cbush[i], 1));
                WDFunc::SetLBLText(m_widget, "bdainvalue" + QString::number(i + 21),
                    WDFunc::StringFloatValueWithCheck(m_blockData->Tg_d[i], 4));
            }
        }

        WDFunc::SetLBLText(m_widget, "bdainvalue24", WDFunc::StringFloatValueWithCheck(m_blockData->Frequency, 4));
        WDFunc::SetLBLText(m_widget, "bdainvalue25", WDFunc::StringFloatValueWithCheck(m_blockData->Pt100_R, 3));
    }
}

BdaIn82::BlockData *BdaIn82::data()
{
    return m_blockData.get();
}
