// checka1.cpp
#include <QGridLayout>
#include "checka1.h"
#include "../publicclass.h"
#include "../widgets/wd_func.h"

Check_A1::Check_A1()
{

}

QWidget *Check_A1::BdaW(QWidget *parent)
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(parent, "1. UeffADC1:"), 0, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value0", ValuesFormat, "Действующее значение напряжения 1 в единицах АЦП"), 0, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "2. UeffADC2:"), 0, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value1", ValuesFormat, "Действующее значение напряжения 2 в единицах АЦП"), 0, 3);
    glyout->addWidget(WDFunc::NewLBL(parent, "3. Freq:"), 1, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value2", ValuesFormat, "Частота сигналов, Гц"), 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "4. Pt100:"), 1, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value3", ValuesFormat, "Значение температурного сигнала"), 1, 3);
    glyout->addWidget(WDFunc::NewLBL(parent, "5. EXTmA1:"), 2, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value4", ValuesFormat, "Значение первого сигнала (4-20) мА"), 2, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "6. EXTmA2:"), 2, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value5", ValuesFormat, "Значение второго сигнала (4-20) мА"), 2, 3);
    w->setLayout(glyout);
    return w;
}

QWidget *Check_A1::Bd1W(const QString &begin, QWidget *parent)
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(parent, "1. UefNat_filt1:"), 0, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"0", ValuesFormat, "Истинное действующее значение напряжения 1 (в В на входе прибора)"), 0, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "2. UefNat_filt2:"), 0, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"1", ValuesFormat, "Истинное действующее значение напряжения 2 (в В на входе прибора)"), 0, 3);
    glyout->addWidget(WDFunc::NewLBL(parent, "3. Uef_filt1:"), 1, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"2", ValuesFormat, "Действующие значения первых гармоник напряжения 1"), 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "4. Uef_filt2:"), 1, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"3", ValuesFormat, "Действующие значения первых гармоник напряжения 2"), 1, 3);
    glyout->addWidget(WDFunc::NewLBL(parent, "5. Phy:"), 2, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"4", ValuesFormat, "Разность фаз первых гармоник напряжений"), 2, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "6. Frequency:"), 2, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"5", ValuesFormat, "Частота в сети"), 2, 3);
    w->setLayout(glyout);
    return w;
}

QWidget *Check_A1::Bd2W(const QString &begin, QWidget *parent)
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QGridLayout *glyout = new QGridLayout;
    for (int i=0; i<61; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "Канал 0, гармоника "+QString::number(i+2)+":"), i, 0);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"0"+QString::number(i), ValuesFormat), i, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Канал 1, гармоника "+QString::number(i+2)+":"), i, 2);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"1"+QString::number(i), ValuesFormat), i, 3);
    }
    w->setLayout(glyout);
    return w;
}

QWidget *Check_A1::Bd3W(const QString &begin, QWidget *parent)
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(parent, "1. Rt:"), 0, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"0", ValuesFormat, "Измеренное сопротивление по каналу Pt100"), 0, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "2. EXTmA1:"), 0, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"1", ValuesFormat, "Измеренный ток по каналу EXTmA1"), 0, 3);
    glyout->addWidget(WDFunc::NewLBL(parent, "3. EXTmA2:"), 0, 4);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"2", ValuesFormat, "Измеренный ток по каналу EXTmA2"), 0, 5);
    w->setLayout(glyout);
    return w;
}

QWidget *Check_A1::Bd4W(const QString &begin, QWidget *parent)
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(parent, "1. Tmk:"), 0, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"0", ValuesFormat, "Температура кристалла микроконтроллера, °С"), 0, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "2. VBAT:"), 0, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"1", ValuesFormat, "Напряжение аккумуляторной батареи, В"), 0, 3);
    glyout->addWidget(WDFunc::NewLBL(parent, "3. Tamb:"), 0, 4);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"2", ValuesFormat, "Температура окружающей среды, °С"), 0, 5);
    glyout->addWidget(WDFunc::NewLBL(parent, "4. Hamb:"), 0, 6);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"3", ValuesFormat, "Относительная влажность окружающей среды, %"), 0, 7);
    w->setLayout(glyout);
    return w;
}

QWidget *Check_A1::Bda_inW(QWidget *parent)
{
    return Bd1W("Bda_in", parent);
}

QWidget *Check_A1::Bda_outW(QWidget *parent)
{
    return Bd1W("Bda_out", parent);
}

QWidget *Check_A1::Bda_hW(QWidget *parent)
{
    return Bd2W("Bda_h", parent);
}

QWidget *Check_A1::Bda_in_anW(QWidget *parent)
{
    return Bd3W("Bda_in_an", parent);
}

QWidget *Check_A1::Bda_out_anW(QWidget *parent)
{
    return Bd4W("Bda_out_an", parent);
}

void Check_A1::FillBda(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "value0", QString::number(Bda_block.Ueff_ADC[0]));
    WDFunc::SetLBLText(parent, "value1", QString::number(Bda_block.Ueff_ADC[1]));
    WDFunc::SetLBLText(parent, "value2", QString::number(Bda_block.Frequency));
    WDFunc::SetLBLText(parent, "value3", QString::number(Bda_block.Pt100));
    WDFunc::SetLBLText(parent, "value4", QString::number(Bda_block.EXTmA1));
    WDFunc::SetLBLText(parent, "value5", QString::number(Bda_block.EXTmA2));
}

void Check_A1::FillBd1W(const QString &begin, A1_Bd1 Bda, QWidget *parent)
{
    WDFunc::SetLBLText(parent, begin+"0", QString::number(Bda.UefNat_filt[0]));
    WDFunc::SetLBLText(parent, begin+"1", QString::number(Bda.UefNat_filt[1]));
    WDFunc::SetLBLText(parent, begin+"2", QString::number(Bda.Uef_filt[0]));
    WDFunc::SetLBLText(parent, begin+"3", QString::number(Bda.Uef_filt[0]));
    WDFunc::SetLBLText(parent, begin+"4", QString::number(Bda.Phy));
    WDFunc::SetLBLText(parent, begin+"5", QString::number(Bda.Frequency));
}

void Check_A1::FillBda_in(QWidget *parent)
{
    FillBd1W("Bda_in", Bda_in, parent);
}

void Check_A1::FillBda_out(QWidget *parent)
{
    FillBd1W("Bda_out", Bda_out, parent);
}

void Check_A1::FillBda_h(QWidget *parent)
{
    for (int i=0; i<61; ++i)
    {
        WDFunc::SetLBLText(parent, "Bda_h0"+QString::number(i), QString::number(Bda_h.HarmBuf[0][i]));
        WDFunc::SetLBLText(parent, "Bda_h1"+QString::number(i), QString::number(Bda_h.HarmBuf[1][i]));
    }
}

void Check_A1::FillBda_in_an(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "Bda_in_an0", QString::number(Bda_in_an.Pt100_R));
    WDFunc::SetLBLText(parent, "Bda_in_an1", QString::number(Bda_in_an.EXTmA1_I));
    WDFunc::SetLBLText(parent, "Bda_in_an2", QString::number(Bda_in_an.EXTmA2_I));
}

void Check_A1::FillBda_out_an(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "Bda_out_an0", QString::number(Bda_out_an.Tmk));
    WDFunc::SetLBLText(parent, "Bda_out_an1", QString::number(Bda_out_an.Vbat));
    WDFunc::SetLBLText(parent, "Bda_out_an2", QString::number(Bda_out_an.Tamb));
    WDFunc::SetLBLText(parent, "Bda_out_an3", QString::number(Bda_out_an.Hamb));
}
