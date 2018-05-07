// checka1.cpp
#include <QGridLayout>
#include <QGroupBox>
#include "check85.h"
#include "../gen/publicclass.h"
#include "../widgets/wd_func.h"

Check_85::Check_85()
{
    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
                "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: "+QString(UCONFCLR)+";}";
}

QWidget *Check_85::Bd1W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    int count = 0;
    glyout->addWidget(WDFunc::NewLBL(parent, "2.Частота:"),count,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value2", ValuesFormat, "Частота сигналов, Гц"),count++,1,1,1);
    lyout->addLayout(hlyout);
    for (i = 0; i < 3; ++i)
    {
        count = 1;
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "IUNF1"+IndexStr),count,i*2,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+3), ValuesFormat, \
                                          QString::number(i+3)+".Истинные действующие значения напряжений 1-й тройки"),count++,i*2+1,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "IUNF2"+IndexStr),count,i*2,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+3), ValuesFormat, \
                                          QString::number(i+6)+".Истинные действующие значения токов"),count++,i*2+1,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "IUNF3"+IndexStr),count,i*2,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+3), ValuesFormat, \
                                          QString::number(i+9)+".Истинные действующие значения напряжений 2-й тройки"),count++,i*2+1,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "UNL1"+IndexStr),count,i*2,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+12), ValuesFormat, \
                                          QString::number(i+12)+".Действующие значения линейных напряжений 1-й тройки"),count++,i*2+1,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "UNL2"+IndexStr),count,i*2,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+15), ValuesFormat, \
                                          QString::number(i+15)+".Действующие значения линейных напряжений 2-й тройки"),count++,i*2+1,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "PNATF"+IndexStr),count,i*2,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+18), ValuesFormat, \
                                          QString::number(i+18)+".Истинная активная мощность"),count++,i*2+1,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "QNATF"+IndexStr),count,i*2,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+21), ValuesFormat, \
                                          QString::number(i+21)+".Реактивная мощность"),count++,i*2+1,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "SNATF"+IndexStr),count,i*2,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+24), ValuesFormat, \
                                          QString::number(i+24)+".Полная мощность"),count++,i*2+1,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "COSPHI"+IndexStr),count,i*2,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+27), ValuesFormat, \
                                          QString::number(i+27)+".Cos φ"),count++,i*2+1,1,1);
    }
    glyout->setColumnStretch(0, 0);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(2, 0);
    glyout->setColumnStretch(3, 10);
    glyout->setColumnStretch(4, 0);
    glyout->setColumnStretch(5, 10);
    lyout->addLayout(glyout);
    glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(parent, "Состояние входных контактных сигналов"),0,0,1,32);
    for (i=0; i<32; ++i)
    {
        QLabel *lbl=new QLabel(QString::number(31-i));
        glyout->addWidget(lbl,1,i,1,1,Qt::AlignCenter);
        QCheckBox *chb = WDFunc::NewChB(parent, "value"+QString::number(i+30), "", ACONFOCLR);
        glyout->addWidget(chb,2,i,1,1,Qt::AlignCenter);
    }
    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

void Check_85::FillBd1(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "value2", WDFunc::StringValueWithCheck(Bd_block1.Frequency, 5));
    for (int i = 0; i < 3; ++i)
    {
        WDFunc::SetLBLText(parent, "value"+QString::number(i+3), WDFunc::StringValueWithCheck(Bd_block1.IUefNat_filt[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+6), WDFunc::StringValueWithCheck(Bd_block1.IUefNat_filt[i+3], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+9), WDFunc::StringValueWithCheck(Bd_block1.IUefNat_filt[i+6], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+12), WDFunc::StringValueWithCheck(Bd_block1.UefNatLin_filt[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+15), WDFunc::StringValueWithCheck(Bd_block1.UefNatLin_filt[i+3], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+18), WDFunc::StringValueWithCheck(Bd_block1.PNatf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+21), WDFunc::StringValueWithCheck(Bd_block1.QNatf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+24), WDFunc::StringValueWithCheck(Bd_block1.SNatf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+27), WDFunc::StringValueWithCheck(Bd_block1.CosPhiNat[i], 5));
    }
    quint32 tmpui = 0x80000000;
    for (int i=0; i<32; ++i)
    {
        if (tmpui & Bd_block1.DD_in)
            WDFunc::SetChBData(parent, "value"+QString::number(i+30), true);
        else
            WDFunc::SetChBData(parent, "value"+QString::number(i+30), false);
    }
}
