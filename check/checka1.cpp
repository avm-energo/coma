// checka1.cpp
#include <QGridLayout>
#include "checka1.h"
#include "../widgets/wd_func.h"

Check_A1::Check_A1()
{

}

QWidget *Check_A1::BdaW(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "1. UeffADC1:"), 0, 0);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "value0", ValuesFormat, "Действующее значение напряжения 1 в единицах АЦП"), 0, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "2. UeffADC2:"), 0, 2);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "value1", ValuesFormat, "Действующее значение напряжения 2 в единицах АЦП"), 0, 3);
    glyout->addWidget(WDFunc::NewLBL(this, "3. Freq:"), 1, 0);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "value2", ValuesFormat, "Частота сигналов, Гц"), 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "4. Pt100:"), 1, 2);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "value3", ValuesFormat, "Значение температурного сигнала"), 1, 3);
    glyout->addWidget(WDFunc::NewLBL(this, "5. EXTmA1:"), 2, 0);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "value4", ValuesFormat, "Значение первого сигнала (4-20) мА"), 2, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "6. EXTmA2:"), 2, 2);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "value5", ValuesFormat, "Значение второго сигнала (4-20) мА"), 2, 3);
    w->setLayout(glyout);
    return w;
}

