#include "tunekiv.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/wd_func.h"

#include <QGroupBox>
#include <QVBoxLayout>

TuneKIV::TuneKIV()
{
    m_BdaTempWidgetIsSet = false;
}

QWidget *TuneKIV::BdaTempWidget()
{
    if (m_BdaTempWidgetIsSet)
        return m_BdaTempWidget;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"
                           "background-color: "
        + QString(Colors::ColorsMap[Colors::AConfO]) + "; font: bold 10px;}";
    m_BdaTempWidget = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    //    QGroupBox *gb = new QGroupBox("Общие данные");
    hlyout->addWidget(WDFunc::NewLBL2(m_BdaTempWidget, "temp"));
    hlyout->addWidget(WDFunc::NewLBLT2(m_BdaTempWidget, "", "temp", ValuesFormat, "Температура"));
    lyout->addLayout(hlyout);
    hlyout->addWidget(WDFunc::NewLBL2(m_BdaTempWidget, "resist"));
    hlyout->addWidget(WDFunc::NewLBLT2(m_BdaTempWidget, "", "resist", ValuesFormat, "Сопротивление"));
    lyout->addLayout(hlyout);
    m_BdaTempWidget->setLayout(lyout);
    m_BdaTempWidgetIsSet = true;
    return m_BdaTempWidget;
}

void TuneKIV::updateBdaTempWidget()
{
    WDFunc::SetLBLText(m_BdaTempWidget, "temp", WDFunc::StringValueWithCheck(m_Bda_temp.temperature, 3));
    WDFunc::SetLBLText(m_BdaTempWidget, "resist", WDFunc::StringValueWithCheck(m_Bda_temp.resistance, 3));
}
