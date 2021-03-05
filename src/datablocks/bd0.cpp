#include "bd0.h"

#include "../gen/colors.h"
#include "../widgets/wd_func.h"

#include <QVBoxLayout>

Bd0::Bd0(QObject *parent) : DataBlock(parent)
{
    m_blockData = new BlockData;
    setBlock({ 0, "Общие", DataTypes::DataBlockTypes::BdBlock, m_blockData, sizeof(BlockData) });
    createWidget();
}

void Bd0::createWidget()
{
    //    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"
    //                           "background-color: "
    //        + QString(Colors::ColorsMap[Colors::AConfO]) + "; font: bold 10px;}";
    m_widget = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(m_widget, "Tmk0"), 0);
    hlyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "101", "Температура процессора"), 5);
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(m_widget, "VBat"), 0);
    hlyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "102", "Напряжение батарейки"), 5);
    lyout->addLayout(hlyout);
    lyout->addStretch(100);
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
    m_widget->setLayout(lyout);
}

void Bd0::setDefBlock()
{
    m_blockData->Tmk = 24;
    m_blockData->Vbat = 3.3f;
}

void Bd0::updateWidget()
{
    WDFunc::SetLBLText(m_widget, "101", WDFunc::StringValueWithCheck(m_blockData->Tmk, 3));
    WDFunc::SetLBLText(m_widget, "102", WDFunc::StringValueWithCheck(m_blockData->Vbat, 3));
}

Bd0::BlockData *Bd0::data()
{
    return m_blockData;
}
