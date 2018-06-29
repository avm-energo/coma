// checka1.cpp
#include <QGridLayout>
#include <QGroupBox>
#include <QtMath>
#include "check80.h"
#include "../config/config.h"
//#include "../gen/publicclass.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../widgets/wd_func.h"

Check_80::Check_80()
{
    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
                "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: "+QString(UCONFCLR)+";}";
}

QWidget *Check_80::Bd1W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(parent, "2.Частота:"));
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "value2", ValuesFormat, "Частота сигналов, Гц"));
    lyout->addLayout(hlyout);
    for (i = 0; i < 6; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "IUNF"+IndexStr),0,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+3), ValuesFormat, \
                                          QString::number(i+3)+".Истинные действующие значения сигналов"),1,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "IUF"+IndexStr),2,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+9), ValuesFormat, \
                                          QString::number(i+9)+".Действующие значения сигналов по 1-й гармонике\nотносительно ф. А 1-й группы"),3,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "PHF"+IndexStr),4,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+15), ValuesFormat, \
                                          QString::number(i+15)+".Угол сдвига между сигналами по первой гармонике\nотносительно ф. А 1-й группы"),5,i,1,1);
    }
    for (i = 0; i < 3; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "PNF"+IndexStr),6,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+21), ValuesFormat, \
                                          QString::number(i+21)+".Истинная активная мощность"),7,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "SNF"+IndexStr),6,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+24), ValuesFormat, \
                                          QString::number(i+24)+".Кажущаяся полная мощность"),7,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "QNF"+IndexStr),8,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+27), ValuesFormat, \
                                          QString::number(i+27)+".Реактивная мощность"),9,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Cos"+IndexStr),8,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+30), ValuesFormat, \
                                          QString::number(i+30)+".Cos phi по истинной активной мощности"),9,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "PF"+IndexStr),10,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+33), ValuesFormat, \
                                          QString::number(i+33)+".Активная мощность по 1-й гармонике"),11,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "QF"+IndexStr),10,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+36), ValuesFormat, \
                                          QString::number(i+36)+".Реактивная мощность по 1-й гармонике"),11,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "SF"+IndexStr),12,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+39), ValuesFormat, \
                                          QString::number(i+39)+".Полная мощность по 1-й гармонике"),13,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "CosPhi"+IndexStr),12,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+42), ValuesFormat, \
                                          QString::number(i+42)+".Cos phi по 1-й гармонике"),13,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "PHI"+IndexStr),14,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+45), ValuesFormat, \
                                          QString::number(i+45)+".Угол между током и напряжением"),15,i,1,1);
    }
    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *Check_80::Bd2W(const QString &title, const QString &begin, QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox(title);
    QGridLayout *glyout = new QGridLayout;
    for (int i = 0; i < 4; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "IUEFNAT_FILT"+IndexStr),0,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+QString::number(i), ValuesFormat, \
                                          QString::number(i)+".Истинные действующие значения сигналов (ABCM)"),1,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "IUEFF_FILTERED"+IndexStr),2,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+QString::number(i+4), ValuesFormat, \
                                          QString::number(i+4)+".Действующие значения сигналов по 1-й гармонике (ABCM)"),3,i,1,1);
    }
    for (int i = 0; i < 3; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "KRF"+IndexStr),4,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+QString::number(i+8), ValuesFormat, \
                                          QString::number(i+8)+".Крест-фактор"),5,i,1,1);
    }
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *Check_80::Bd4W(const QString &title, const QString &begin, QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox(title);
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(parent, "U0"),0,0,1,1,Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"U0", ValuesFormat, "Сигнал нулевой последовательности"),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U1"),0,2,1,1,Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"U1", ValuesFormat, "Сигнал прямой последовательности"),0,3,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U2"),0,4,1,1,Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"U2", ValuesFormat, "Сигнал обратной последовательности"),0,5,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Kunsim0"),1,0,1,1,Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"Kunsim0", ValuesFormat, "Коэф. несимм. по нулевой последовательности"),1,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Kunsim2"),1,2,1,1,Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"Kunsim2", ValuesFormat, "Коэф. несимм. по обратной последовательности"),1,3,1,1);
    for (int i = 0; i < 3; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "THD"+IndexStr),2,i*2,1,1,Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"THD"+QString::number(i), ValuesFormat, "Коэфф. гарм. искажений ф. "+QString::number(i+10,16)),2,i*2+1,1,1);
    }
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *Check_80::Bd5W(const QString &title, const QString &begin, QWidget *parent)
{
    int Beg = (begin.at(begin.size()-1).digitValue())*15;
    int End = Beg + 15;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox(title);
    QGridLayout *glyout = new QGridLayout;
    for (int i=Beg; i<End; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "Ф А, Гм "+QString::number(i+2)+":"), i, 0);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"0"+QString::number(i), ValuesFormat), i, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Ф В, Гм "+QString::number(i+2)+":"), i, 2);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"1"+QString::number(i), ValuesFormat), i, 3);
        glyout->addWidget(WDFunc::NewLBL(parent, "Ф С, Гм "+QString::number(i+2)+":"), i, 4);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+"2"+QString::number(i), ValuesFormat), i, 5);
    }
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    glyout->setColumnStretch(5, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *Check_80::Bd8W(const QString &title, const QString &begin, QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox(title);
    QGridLayout *glyout = new QGridLayout;
    for (int i = 0; i < 4; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "IUEFNATLIN_FILT"+IndexStr),0,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+QString::number(i), ValuesFormat, \
                                          "Истинные действующие значения линейных напряжений"),1,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "IUEFFLIN_FILTERED"+IndexStr),2,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", begin+QString::number(i+4), ValuesFormat, \
                                          "Действующие значения 1-й гармонике линейных напряжений"),3,i,1,1);
    }
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *Check_80::Bd10W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    for (i = 0; i < 4; ++i)
    {
        QString IndexStr = QString::number(i);
        glyout->addWidget(WDFunc::NewLBL(parent, "A"), 0, 1, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "B"), 0, 2, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "C"), 0, 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "O"), 0, 4, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "PNATF:"), 1, 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "B10PNATF"+IndexStr, ValuesFormat, "Истинная активная мощность"), 1, i+1, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "QNATF:"), 2, 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "B10QNATF", ValuesFormat, "Реактивная мощность"), 2, i+1, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "SNATF:"), 3, 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "B10SNATF", ValuesFormat, "Полная мощность"), 3, i+1, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "COSPHINAT:"), 4, 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "B10COSPHINAT", ValuesFormat, "Cos phi"), 4, i+1, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "PF:"), 5, 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "B10PF"+IndexStr, ValuesFormat, "Активная мощность по 1 гарм."), 5, i+1, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "QF:"), 6, 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "B10QF", ValuesFormat, "Реактивная мощность по 1 гарм."), 6, i+1, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "SF:"), 7, 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "B10SF", ValuesFormat, "Полная мощность по 1 гарм."), 7, i+1, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "COSPHI:"), 8, 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "B10COSPHI", ValuesFormat, "Cos phi по 1 гарм."), 8, i+1, 1, 1);
    }
    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
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

QWidget *Check_80::Bd_4W(QWidget *parent)
{
    return Bd4W("Данные по несимметрии и искажениям 1-й тройки", "Bd_4", parent);
}

QWidget *Check_80::Bd_5W(int num, QWidget *parent)
{
    return Bd5W("Данные по гармоническому составу 1-й тройки", "Bd_5"+QString::number(num), parent);
}

QWidget *Check_80::Bd_6W(QWidget *parent)
{
    return Bd4W("Данные по несимметрии и искажениям 2-й тройки", "Bd_6", parent);
}

QWidget *Check_80::Bd_7W(int num, QWidget *parent)
{
    return Bd5W("Данные по гармоническому составу 2-й тройки", "Bd_7"+QString::number(num), parent);
}

QWidget *Check_80::Bd_8W(QWidget *parent)
{
    return Bd8W("Линейные напряжения 1-й тройки в кВ", "Bd_8", parent);
}

QWidget *Check_80::Bd_9W(QWidget *parent)
{
    return Bd8W("Линейные напряжения 2-й тройки в кВ", "Bd_9", parent);
}

QWidget *Check_80::Bd_10W(QWidget *parent)
{
    return Bd10W(parent);
}

void Check_80::FillBd2W(const QString &begin, Check_80::Bd2 &Bd, QWidget *parent)
{
    for (int i = 0; i < 4; ++i)
    {
        WDFunc::SetLBLText(parent, begin+QString::number(i), WDFunc::StringValueWithCheck(Bd.IUefNat_filt[i], 4));
        WDFunc::SetLBLText(parent, begin+QString::number(i+4), WDFunc::StringValueWithCheck(Bd.IUeff_filtered[i], 4));
        if (i < 3)
            WDFunc::SetLBLText(parent, begin+QString::number(i+8), WDFunc::StringValueWithCheck(Bd.KrF[i], 5));
    }
}

void Check_80::FillBd4W(const QString &begin, Check_80::Bd4 &Bd, QWidget *parent)
{
    WDFunc::SetLBLText(parent, begin+"U0", WDFunc::StringValueWithCheck(Bd.U0, 4));
    WDFunc::SetLBLText(parent, begin+"U1", WDFunc::StringValueWithCheck(Bd.U1, 4));
    WDFunc::SetLBLText(parent, begin+"U2", WDFunc::StringValueWithCheck(Bd.U2, 4));
    WDFunc::SetLBLText(parent, begin+"Kunsim0", WDFunc::StringValueWithCheck(Bd.Kunsim0, 4));
    WDFunc::SetLBLText(parent, begin+"Kunsim2", WDFunc::StringValueWithCheck(Bd.Kunsim2, 4));
    for (int i = 0; i < 3; ++i)
        WDFunc::SetLBLText(parent, begin+"THD"+QString::number(i), WDFunc::StringValueWithCheck(Bd.THD[i], 5));
}

void Check_80::FillBd5W(const QString &begin, Check_80::Bd5 &Bd, QWidget *parent)
{
    for (int i=0; i<16; ++i)
    {
        QString begins = begin + "0";
        WDFunc::SetLBLText(parent, begin+"0"+QString::number(i), WDFunc::StringValueWithCheck(Bd.Harm[0][i]));
        WDFunc::SetLBLText(parent, begin+"1"+QString::number(i), WDFunc::StringValueWithCheck(Bd.Harm[1][i]));
        WDFunc::SetLBLText(parent, begin+"2"+QString::number(i), WDFunc::StringValueWithCheck(Bd.Harm[2][i]));
        int count = 16+i;
        begins = begin + "1";
        WDFunc::SetLBLText(parent, begin+"0"+QString::number(count), WDFunc::StringValueWithCheck(Bd.Harm[0][count]));
        WDFunc::SetLBLText(parent, begin+"1"+QString::number(count), WDFunc::StringValueWithCheck(Bd.Harm[1][count]));
        WDFunc::SetLBLText(parent, begin+"2"+QString::number(count), WDFunc::StringValueWithCheck(Bd.Harm[2][count]));
        count = 32+i;
        begins = begin + "2";
        WDFunc::SetLBLText(parent, begin+"0"+QString::number(count), WDFunc::StringValueWithCheck(Bd.Harm[0][count]));
        WDFunc::SetLBLText(parent, begin+"1"+QString::number(count), WDFunc::StringValueWithCheck(Bd.Harm[1][count]));
        WDFunc::SetLBLText(parent, begin+"2"+QString::number(count), WDFunc::StringValueWithCheck(Bd.Harm[2][count]));
        count = 48+i;
        if (count > C80_MAXHARMINDEX)
            continue;
        begins = begin + "3";
        WDFunc::SetLBLText(parent, begin+"0"+QString::number(count), WDFunc::StringValueWithCheck(Bd.Harm[0][count]));
        WDFunc::SetLBLText(parent, begin+"1"+QString::number(count), WDFunc::StringValueWithCheck(Bd.Harm[1][count]));
        WDFunc::SetLBLText(parent, begin+"2"+QString::number(count), WDFunc::StringValueWithCheck(Bd.Harm[2][count]));
    }
}

void Check_80::FillBd8W(const QString &begin, Check_80::Bd8 &Bd, QWidget *parent)
{
    for (int i = 0; i < 4; ++i)
    {
        WDFunc::SetLBLText(parent, begin+QString::number(i), WDFunc::StringValueWithCheck(Bd.UefNatLin_filt[i], 3));
        WDFunc::SetLBLText(parent, begin+QString::number(i+4), WDFunc::StringValueWithCheck(Bd.UeffLin_filtered[i], 3));
    }
}

void Check_80::FillBd1(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "value2", WDFunc::StringValueWithCheck(Bd_block1.Frequency, 3));
    for (int i = 0; i < 3; i++)
    {
        int Precision = (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) != MTM_81) ? 3 : 4;
        WDFunc::SetLBLText(parent, "value"+QString::number(i+3), WDFunc::StringValueWithCheck(Bd_block1.IUefNat_filt[i], Precision));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+9), WDFunc::StringValueWithCheck(Bd_block1.IUeff_filtered[i], Precision));
        Precision = (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) != MTM_83) ? 4 : 3;
        WDFunc::SetLBLText(parent, "value"+QString::number(i+6), WDFunc::StringValueWithCheck(Bd_block1.IUefNat_filt[i+3], Precision));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+12), WDFunc::StringValueWithCheck(Bd_block1.IUeff_filtered[i+3], Precision));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+15), WDFunc::StringValueWithCheck(Bd_block1.phi_next_f[i], 4));
    }
    for (int i=0; i<3; i++)
    {
        WDFunc::SetLBLText(parent, "value"+QString::number(i+21), WDFunc::StringValueWithCheck(Bd_block1.PNatf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+24), WDFunc::StringValueWithCheck(Bd_block1.SNatf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+27), WDFunc::StringValueWithCheck(Bd_block1.QNatf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+30), WDFunc::StringValueWithCheck(Bd_block1.CosPhiNat[i], 4));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+33), WDFunc::StringValueWithCheck(Bd_block1.Pf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+36), WDFunc::StringValueWithCheck(Bd_block1.Qf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+39), WDFunc::StringValueWithCheck(Bd_block1.Sf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+42), WDFunc::StringValueWithCheck(Bd_block1.CosPhi[i], 4));
        float PHI = (180*qAsin(Bd_block1.Qf[i]/Bd_block1.Sf[i])/M_PI);
        WDFunc::SetLBLText(parent, "value"+QString::number(i+45), WDFunc::StringValueWithCheck(PHI, 4));
    }
}

void Check_80::FillBd2(QWidget *parent)
{
    FillBd2W("Bd_2", Bd_block2, parent);
}

void Check_80::FillBd3(QWidget *parent)
{
    FillBd2W("Bd_3", Bd_block3, parent);
}

void Check_80::FillBd4(QWidget *parent)
{
    FillBd4W("Bd_4", Bd_block4, parent);
}

void Check_80::FillBd5(QWidget *parent)
{
    FillBd5W("Bd_5", Bd_block5, parent);
}

void Check_80::FillBd6(QWidget *parent)
{
    FillBd4W("Bd_6", Bd_block6, parent);
}

void Check_80::FillBd7(QWidget *parent)
{
    FillBd5W("Bd_7", Bd_block7, parent);
}

void Check_80::FillBd8(QWidget *parent)
{
    FillBd8W("Bd_8", Bd_block8, parent);
}

void Check_80::FillBd9(QWidget *parent)
{
    FillBd8W("Bd_9", Bd_block9, parent);
}

void Check_80::FillBd10(QWidget *parent)
{
    for (int i = 0; i < 4; ++i)
    {
        WDFunc::SetLBLText(parent, "B10PNATF"+QString::number(i), WDFunc::StringValueWithCheck(Bd_block10.PNatf[i], 3));
        WDFunc::SetLBLText(parent, "B10QNATF"+QString::number(i), WDFunc::StringValueWithCheck(Bd_block10.QNatf[i], 3));
        WDFunc::SetLBLText(parent, "B10SNATF"+QString::number(i), WDFunc::StringValueWithCheck(Bd_block10.SNatf[i], 3));
        WDFunc::SetLBLText(parent, "B10COSPHINAT"+QString::number(i), WDFunc::StringValueWithCheck(Bd_block10.CosPhiNat[i], 5));
        WDFunc::SetLBLText(parent, "B10PF"+QString::number(i), WDFunc::StringValueWithCheck(Bd_block10.Pf[i], 3));
        WDFunc::SetLBLText(parent, "B10QF"+QString::number(i), WDFunc::StringValueWithCheck(Bd_block10.Qf[i], 3));
        WDFunc::SetLBLText(parent, "B10SF"+QString::number(i), WDFunc::StringValueWithCheck(Bd_block10.Sf[i], 3));
        WDFunc::SetLBLText(parent, "B10COSPHI"+QString::number(i), WDFunc::StringValueWithCheck(Bd_block10.CosPhi[i], 5));
    }
}
