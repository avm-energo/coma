// checka1.cpp
#include <QGridLayout>
#include <QGroupBox>
#include "check80.h"
#include "../publicclass.h"
#include "../widgets/wd_func.h"

Check_80::Check_80()
{

}

QWidget *Check_80::Bd1W(QWidget *parent)
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "0.Температура:"));
    hlyout->addWidget(WDFunc::NewLBLT(this, "", "value0", ValuesFormat, "Температура кристалла микроконтроллера, °С"));
    hlyout->addWidget(WDFunc::NewLBL(this, "1.Батарея:"));
    hlyout->addWidget(WDFunc::NewLBLT(this, "", "value1", ValuesFormat, "Напряжение аккумуляторной батареи, В"));
    hlyout->addWidget(WDFunc::NewLBL(this, "2.Частота:"));
    hlyout->addWidget(WDFunc::NewLBLT(this, "", "value2", ValuesFormat, "Частота сигналов, Гц"));
    lyout->addLayout(hlyout);
    for (i = 0; i < 6; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(this, "IUNF"+IndexStr),0,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+3), ValuesFormat, \
                                          QString::number(i+3)+".Истинные действующие значения сигналов"),1,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "IUF"+IndexStr),2,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+9), ValuesFormat, \
                                          QString::number(i+9)+".Действующие значения сигналов по 1-й гармонике\nотносительно ф. А 1-й группы"),3,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "PHF"+IndexStr),4,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+15), ValuesFormat, \
                                          QString::number(i+15)+".Угол сдвига между сигналами по первой гармонике\nотносительно ф. А 1-й группы"),5,i,1,1);
    }
    for (i = 0; i < 3; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(this, "PNF"+IndexStr),6,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+21), ValuesFormat, \
                                          QString::number(i+21)+".Истинная активная мощность"),7,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "SNF"+IndexStr),6,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+24), ValuesFormat, \
                                          QString::number(i+24)+".Кажущаяся полная мощность"),7,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "QNF"+IndexStr),8,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+27), ValuesFormat, \
                                          QString::number(i+27)+".Реактивная мощность"),9,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "Cos"+IndexStr),8,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+30), ValuesFormat, \
                                          QString::number(i+30)+".Cos phi по истинной активной мощности"),9,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "PF"+IndexStr),10,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+33), ValuesFormat, \
                                          QString::number(i+33)+".Активная мощность по 1-й гармонике"),11,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "QF"+IndexStr),10,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+36), ValuesFormat, \
                                          QString::number(i+36)+".Реактивная мощность по 1-й гармонике"),11,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "SF"+IndexStr),12,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+39), ValuesFormat, \
                                          QString::number(i+39)+".Полная мощность по 1-й гармонике"),13,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "CosPhi"+IndexStr),12,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+42), ValuesFormat, \
                                          QString::number(i+42)+".Cos phi по 1-й гармонике"),13,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "PHI"+IndexStr),14,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+45), ValuesFormat, \
                                          QString::number(i+45)+".Угол между током и напряжением"),15,i,1,1);
    }
    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

QWidget *Check_80::Bd2W(const QString &title, const QString &begin, QWidget *parent)
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox(title);
    QGridLayout *glyout = new QGridLayout;
    for (int i = 0; i < 4; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(this, "IUEFNAT_FILT"+IndexStr),0,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", begin+QString::number(i), ValuesFormat, \
                                          QString::number(i)+".Истинные действующие значения сигналов"),1,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "IUEFF_FILTERED"+IndexStr),2,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", begin+QString::number(i+4), ValuesFormat, \
                                          QString::number(i+4)+".Действующие значения сигналов по 1-й гармонике"),3,i,1,1);
    }
    for (int i = 0; i < 4; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(this, "KRF"+IndexStr),4,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", begin+QString::number(i+8), ValuesFormat, \
                                          QString::number(i+8)+".Крест-фактор"),5,i,1,1);
    }
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

QWidget *Check_80::Bd4W(const QString &title, const QString &begin, QWidget *parent)
{
    int Beg = (begin.at(begin.size()-1).digitValue())*15;
    int End = Beg + 15;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QGridLayout *glyout = new QGridLayout;
    for (int i=Beg; i<End; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "Канал 0, гарм. "+QString::number(i+2)+":"), i, 0);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"0"+QString::number(i), ValuesFormat), i, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Канал 1, гарм. "+QString::number(i+2)+":"), i, 2);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"1"+QString::number(i), ValuesFormat), i, 3);
    }
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    w->setLayout(glyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;

    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(parent, "1. Tmk:"), 0, 0);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"0", ValuesFormat, ""), 0, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "2. VBAT:"), 0, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"1", ValuesFormat, ""), 0, 3);
    glyout->addWidget(WDFunc::NewLBL(parent, "3. Tamb:"), 0, 4);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"2", ValuesFormat, "Температура окружающей среды, °С"), 0, 5);
    glyout->addWidget(WDFunc::NewLBL(parent, "4. Hamb:"), 0, 6);
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

QWidget *Check_80::Bd_1W(QWidget *parent)
{
    return Bd1W(parent);
}

QWidget *Check_80::Bd_2W(QWidget *parent)
{
    return Bd2W("Действующие значения сигналов 1-й тройки", "Bd_2", parent);
}

QWidget *Check_80::Bd_3W(QWidget *parent)
{
    return Bd2W("Действующие значения сигналов 2-й тройки", "Bd_3", parent);
}

QWidget *Check_80::Bd_4aW(QWidget *parent)
{
    return Bd4W("Bd_4", parent);
}

QWidget *Check_80::Bd_5W(QWidget *parent)
{
    return Bd4W("Bda_h2", parent);
}

QWidget *Check_80::Bda_h3W(QWidget *parent)
{
    return Bd2W("Bda_h3", parent);
}

QWidget *Check_80::Bda_in_anW(QWidget *parent)
{
    return Bd3W("Bda_in_an", parent);
}

QWidget *Check_80::Bda_out_anW(QWidget *parent)
{
    return Bd4W("Bda_out_an", parent);
}

void Check_80::FillBd1W(const QString &begin, A1_Bd1 Bda, QWidget *parent)
{
    WDFunc::SetLBLText(parent, begin+"0", WDFunc::StringValueWithCheck(Bda.UefNat_filt[0]));
    WDFunc::SetLBLText(parent, begin+"1", WDFunc::StringValueWithCheck(Bda.UefNat_filt[1]));
    WDFunc::SetLBLText(parent, begin+"2", WDFunc::StringValueWithCheck(Bda.Uef_filt[0]));
    WDFunc::SetLBLText(parent, begin+"3", WDFunc::StringValueWithCheck(Bda.Uef_filt[0]));
    WDFunc::SetLBLText(parent, begin+"4", WDFunc::StringValueWithCheck(Bda.Phy));
    WDFunc::SetLBLText(parent, begin+"5", WDFunc::StringValueWithCheck(Bda.Frequency));
}

void Check_80::FillBda_in(QWidget *parent)
{
    FillBd1W("Bda_in", Bda_in, parent);
}

void Check_80::FillBda_out(QWidget *parent)
{
    FillBd1W("Bda_out", Bda_out, parent);
}

void Check_80::FillBda_h(QWidget *parent)
{
    for (int i=0; i<15; ++i)
    {
        WDFunc::SetLBLText(parent, "Bda_h00"+QString::number(i), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[0][i]));
        WDFunc::SetLBLText(parent, "Bda_h01"+QString::number(i), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[1][i]));
        WDFunc::SetLBLText(parent, "Bda_h10"+QString::number(15+i), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[0][15+i]));
        WDFunc::SetLBLText(parent, "Bda_h11"+QString::number(15+i), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[1][15+i]));
        WDFunc::SetLBLText(parent, "Bda_h20"+QString::number(30+i), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[0][30+i]));
        WDFunc::SetLBLText(parent, "Bda_h21"+QString::number(30+i), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[1][30+i]));
        WDFunc::SetLBLText(parent, "Bda_h30"+QString::number(45+i), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[0][45+i]));
        WDFunc::SetLBLText(parent, "Bda_h31"+QString::number(45+i), WDFunc::StringValueWithCheck(Bda_h.HarmBuf[1][45+i]));
    }
}

void Check_80::FillBda_in_an(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "Bda_in_an0", WDFunc::StringValueWithCheck(Bda_in_an.Pt100_R));
    WDFunc::SetLBLText(parent, "Bda_in_an1", WDFunc::StringValueWithCheck(Bda_in_an.EXTmA1_I));
    WDFunc::SetLBLText(parent, "Bda_in_an2", WDFunc::StringValueWithCheck(Bda_in_an.EXTmA2_I));
}

void Check_80::FillBda_out_an(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "Bda_out_an0", WDFunc::StringValueWithCheck(Bda_out_an.Tmk));
    WDFunc::SetLBLText(parent, "Bda_out_an1", WDFunc::StringValueWithCheck(Bda_out_an.Vbat));
    WDFunc::SetLBLText(parent, "Bda_out_an2", WDFunc::StringValueWithCheck(Bda_out_an.Tamb));
    WDFunc::SetLBLText(parent, "Bda_out_an3", WDFunc::StringValueWithCheck(Bda_out_an.Hamb));
}
