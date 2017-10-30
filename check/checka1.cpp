// checka1.cpp
#include <QGridLayout>
#include <QGroupBox>
#include "checka1.h"
#include "../publicclass.h"
#include "../widgets/wd_func.h"

CheckA1::CheckA1()
{

}

QWidget *CheckA1::BdaW(QWidget *parent)
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
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    w->setLayout(glyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

QWidget *CheckA1::Bd1W(const QString &title, const QString &begin, QWidget *parent)
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(parent, "0. Вариант использования: "));
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"00", ValuesFormat));
    lyout->addLayout(hlyout);
    QGroupBox *gb = new QGroupBox(title);
    QGridLayout *glyout = new QGridLayout;
    QString tmps = (begin == "Bda_out") ? "кВ" : "В";
    glyout->addWidget(WDFunc::NewLBL(parent, "1. UefNat_filt1, "+tmps), 0, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"0", ValuesFormat, "Истинное действующее значение напряжения 1 (в В на входе прибора), В"), 0, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "2. UefNat_filt2, "+tmps), 0, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"1", ValuesFormat, "Истинное действующее значение напряжения 2 (в В на входе прибора), В"), 0, 3);
    glyout->addWidget(WDFunc::NewLBL(parent, "δUrms, %"), 0, 4);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"dU", ValuesFormat, "Относительная погрешность, %"), 0, 5);
    glyout->addWidget(WDFunc::NewLBL(parent, "3. Uef_filt1, "+tmps), 1, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"2", ValuesFormat, "Действующие значения первых гармоник напряжения 1, В"), 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "4. Uef_filt2, "+tmps), 1, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"3", ValuesFormat, "Действующие значения первых гармоник напряжения 2, В"), 1, 3);
    glyout->addWidget(WDFunc::NewLBL(parent, "5. δU, %"), 1, 4);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"dU2", ValuesFormat, "Относительная погрешность, %"), 1, 5);
    glyout->addWidget(WDFunc::NewLBL(parent, "6. Phy, срад"), 2, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"4", ValuesFormat, "Разность фаз первых гармоник напряжений, срад"), 2, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "7. Frequency, Гц"), 2, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"5", ValuesFormat, "Частота в сети, Гц"), 2, 3);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    glyout->setColumnStretch(5, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addStretch(100);
    w->setLayout(lyout);
    tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

QWidget *CheckA1::Bd2W(const QString &begin, QWidget *parent)
{
    int Beg = (begin.at(begin.size()-1).digitValue())*16;
    int End = Beg + 16;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QGridLayout *glyout = new QGridLayout;
    for (int i=Beg; i<End; ++i)
    {
        if (i == 0)
        {
            glyout->addWidget(WDFunc::NewLBL(parent, "Канал 0, КГИ, %"), i, 0);
            glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"0"+QString::number(i), ValuesFormat), i, 1);
            glyout->addWidget(WDFunc::NewLBL(parent, "Канал 1, КГИ, %"), i, 2);
            glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"1"+QString::number(i), ValuesFormat), i, 3);
        }
        else if (i > MAXHARMINDEX)
            break;
        else
        {
            glyout->addWidget(WDFunc::NewLBL(parent, "Канал 0, гарм. "+QString::number(i+1) + ", %"), i, 0);
            glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"0"+QString::number(i), ValuesFormat), i, 1);
            glyout->addWidget(WDFunc::NewLBL(parent, "Канал 1, гарм. "+QString::number(i+1) + ", %"), i, 2);
            glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"1"+QString::number(i), ValuesFormat), i, 3);
        }
    }
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    w->setLayout(glyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

QWidget *CheckA1::Bd3W(const QString &begin, QWidget *parent)
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(parent, "1. Rt, Ом"), 0, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"0", ValuesFormat, "Измеренное сопротивление по каналу Pt100"), 0, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "2. EXTmA1, мА"), 0, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"1", ValuesFormat, "Измеренный ток по каналу EXTmA1"), 0, 3);
    glyout->addWidget(WDFunc::NewLBL(parent, "3. EXTmA2, мА"), 0, 4);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"2", ValuesFormat, "Измеренный ток по каналу EXTmA2"), 0, 5);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    glyout->setColumnStretch(5, 10);
    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

QWidget *CheckA1::Bd4W(const QString &begin, QWidget *parent)
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(parent, "1. Tmk, °С:"), 0, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"0", ValuesFormat, "Температура кристалла микроконтроллера, °С"), 0, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "2. VBAT, В:"), 0, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"1", ValuesFormat, "Напряжение аккумуляторной батареи, В"), 0, 3);
    glyout->addWidget(WDFunc::NewLBL(parent, "3. Tamb, °С:"), 0, 4);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"2", ValuesFormat, "Температура окружающей среды, °С"), 0, 5);
    glyout->addWidget(WDFunc::NewLBL(parent, "4. Hamb, %:"), 0, 6);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"3", ValuesFormat, "Относительная влажность окружающей среды, %"), 0, 7);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    glyout->setColumnStretch(5, 10);
    glyout->setColumnStretch(7, 10);
    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

QWidget *CheckA1::Bda_inW(QWidget *parent)
{
    return Bd1W("Переменные напряжения на входах", "Bda_in", parent);
}

QWidget *CheckA1::Bda_outW(QWidget *parent)
{
    return Bd1W("Переменные напряжения в первичном масштабе", "Bda_out", parent);
}

QWidget *CheckA1::Bda_h0W(QWidget *parent)
{
    return Bd2W("Bda_h0", parent);
}

QWidget *CheckA1::Bda_h1W(QWidget *parent)
{
    return Bd2W("Bda_h1", parent);
}

QWidget *CheckA1::Bda_h2W(QWidget *parent)
{
    return Bd2W("Bda_h2", parent);
}

QWidget *CheckA1::Bda_h3W(QWidget *parent)
{
    return Bd2W("Bda_h3", parent);
}

QWidget *CheckA1::Bda_in_anW(QWidget *parent)
{
    return Bd3W("Bda_in_an", parent);
}

QWidget *CheckA1::Bda_out_anW(QWidget *parent)
{
    return Bd4W("Bda_out_an", parent);
}

void CheckA1::FillBda(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "value0", WDFunc::StringValueWithCheck(Bda_block.Ueff_ADC[0]));
    WDFunc::SetLBLText(parent, "value1", WDFunc::StringValueWithCheck(Bda_block.Ueff_ADC[1]));
    WDFunc::SetLBLText(parent, "value2", WDFunc::StringValueWithCheck(Bda_block.Frequency));
    WDFunc::SetLBLText(parent, "value3", WDFunc::StringValueWithCheck(Bda_block.Pt100));
    WDFunc::SetLBLText(parent, "value4", WDFunc::StringValueWithCheck(Bda_block.EXTmA1));
    WDFunc::SetLBLText(parent, "value5", WDFunc::StringValueWithCheck(Bda_block.EXTmA2));
}

void CheckA1::FillBd1W(const QString &begin, A1_Bd1 Bda, QWidget *parent)
{
    WDFunc::SetLBLText(parent, begin+"0", WDFunc::StringValueWithCheck(Bda.UefNat_filt[0]));
    WDFunc::SetLBLText(parent, begin+"1", WDFunc::StringValueWithCheck(Bda.UefNat_filt[1]));
    WDFunc::SetLBLText(parent, begin+"2", WDFunc::StringValueWithCheck(Bda.Uef_filt[0]));
    WDFunc::SetLBLText(parent, begin+"3", WDFunc::StringValueWithCheck(Bda.Uef_filt[1]));
    WDFunc::SetLBLText(parent, begin+"4", WDFunc::StringValueWithCheck(Bda.Phy));
    WDFunc::SetLBLText(parent, begin+"5", WDFunc::StringValueWithCheck(Bda.Frequency));
//    WDFunc::SetLBLText(parent, begin+"dU", QString::number(qAbs(Bda.UefNat_filt[0]-Bda.UefNat_filt[1])/57.74f*100));
//    WDFunc::SetLBLText(parent, begin+"dU2", QString::number(qAbs(Bda.Uef_filt[0]-Bda.Uef_filt[1])/57.74f*100));
    WDFunc::SetLBLText(parent, begin+"dU", WDFunc::StringValueWithCheck(Bda.dUrms));
    WDFunc::SetLBLText(parent, begin+"dU2", WDFunc::StringValueWithCheck(Bda.dU));
}

void CheckA1::FillBda_in(QWidget *parent)
{
    FillBd1W("Bda_in", Bda_in, parent);
}

void CheckA1::FillBda_out(QWidget *parent)
{
    FillBd1W("Bda_out", Bda_out, parent);
}

void CheckA1::FillBda_h(QWidget *parent)
{
    for (int i=0; i<16; ++i)
    {
        WDFunc::SetLBLText(parent, "Bda_h00"+QString::number(i), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[0][i]));
        WDFunc::SetLBLText(parent, "Bda_h01"+QString::number(i), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[1][i]));
        int count = 16+i;
        WDFunc::SetLBLText(parent, "Bda_h10"+QString::number(count), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[0][count]));
        WDFunc::SetLBLText(parent, "Bda_h11"+QString::number(count), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[1][count]));
        count = 32+i;
        WDFunc::SetLBLText(parent, "Bda_h20"+QString::number(count), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[0][count]));
        WDFunc::SetLBLText(parent, "Bda_h21"+QString::number(count), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[1][count]));
        count = 48+i;
        if (count > MAXHARMINDEX)
            continue;
        WDFunc::SetLBLText(parent, "Bda_h30"+QString::number(count), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[0][count]));
        WDFunc::SetLBLText(parent, "Bda_h31"+QString::number(count), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[1][count]));
    }
}

void CheckA1::FillBda_in_an(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "Bda_in_an0", WDFunc::StringValueWithCheck(Bda_in_an.Pt100_R));
    WDFunc::SetLBLText(parent, "Bda_in_an1", WDFunc::StringValueWithCheck(Bda_in_an.EXTmA1_I));
    WDFunc::SetLBLText(parent, "Bda_in_an2", WDFunc::StringValueWithCheck(Bda_in_an.EXTmA2_I));
}

void CheckA1::FillBda_out_an(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "Bda_out_an0", WDFunc::StringValueWithCheck(Bda_out_an.Tmk));
    WDFunc::SetLBLText(parent, "Bda_out_an1", WDFunc::StringValueWithCheck(Bda_out_an.Vbat));
    WDFunc::SetLBLText(parent, "Bda_out_an2", WDFunc::StringValueWithCheck(Bda_out_an.Tamb));
    WDFunc::SetLBLText(parent, "Bda_out_an3", WDFunc::StringValueWithCheck(Bda_out_an.Hamb));
}
