#pragma once

#include "bda.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/wd_func.h"

#include <QGroupBox>
#include <QVBoxLayout>

Bda::Bda(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 0, "Текущие данные", DataTypes::DataBlockTypes::BdaBlock, m_blockData.get(), sizeof(BlockData) });
    createWidget();
}

void Bda::createWidget()
{
    m_widget = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    //    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Данные без регулировки");
    for (int i = 0; i < 6; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "Ueff_ADC" + IndexStr), 0, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "bdavalue" + QString::number(i),
                              "Ueff_ADC" + IndexStr + ".Измеренные сигналы в кодах АЦП"),
            1, i, 1, 1);
    }
    glyout->addWidget(WDFunc::NewLBL2(m_widget, "Frequency"), 2, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "bdavalue6", "Частота"), 3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(m_widget, "Pt100"), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "bdavalue7", "Температура"), 3, 1, 1, 1);
    //    ETableView *tv = new ETableView;
    //    bdavalueDelegate *chdg = new bdavalueDelegate;
    //    tv->setItemDelegate(chdg);
    //    for (int i = 0; i < 6; i++)
    //    {
    //        m_VModel->setData(m_VModel->index(0, i * 2), "Ueff_ADC[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(0, i * 2 + 1), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(0, i * 2 + 1), &m_Bda.Ueff_ADC[i]);
    //    }
    //    m_VModel->setData(m_VModel->index(1, 0), "Frequency");
    //    m_VModel->setData(m_VModel->index(1, 1), ValueItem::OUTVALUEINT);
    //    m_VModel->setValueData(m_VModel->index(1, 1), &m_Bda.Frequency);
    //    m_VModel->setData(m_VModel->index(1, 2), "Pt100");
    //    m_VModel->setData(m_VModel->index(1, 3), ValueItem::OUTVALUEINT);
    //    m_VModel->setValueData(m_VModel->index(1, 3), &m_Bda.Pt100);
    //    tv->setModel(m_VModel);
    //    vlyout->addWidget(tv);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addStretch(100);
    lyout->addStretch(100);
    m_widget->setLayout(lyout);
    m_widgetIsSet = true;
}

void Bda::setDefBlock()
{
}

void Bda::updateWidget()
{
    if (m_widgetIsSet)
    {
        for (int i = 0; i < 6; i++)
        {
            WDFunc::SetLBLText(
                m_widget, "bdavalue" + QString::number(i), WDFunc::StringFloatValueWithCheck(m_blockData->Ueff_ADC[i], 4));
        }

        WDFunc::SetLBLText(m_widget, "bdavalue6", WDFunc::StringFloatValueWithCheck(m_blockData->Frequency, 4));
        WDFunc::SetLBLText(m_widget, "bdavalue7", WDFunc::StringFloatValueWithCheck(m_blockData->Pt100, 4));
    }
}

Bda::BlockData *Bda::data()
{
    return m_blockData.get();
}
