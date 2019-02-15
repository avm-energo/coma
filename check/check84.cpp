#include <QGridLayout>
#include <QGroupBox>
#include <QtMath>
#include "check84.h"
#include "../config/config.h"
//#include "../gen/publicclass.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../widgets/wd_func.h"

Check_84::Check_84()
{
    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
                "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: "+QString(UCONFCLR)+";}";
}

QWidget *Check_84::Bd1W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(parent, "Номер:"), 0);
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "value0", ValuesFormat, "Номер"), 0);
    hlyout->addWidget(WDFunc::NewLBL(parent, "Температура микроконтроллера, °С:"), 0);
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "value1", ValuesFormat, "Температура микроконтроллера, °С"), 0);
    hlyout->addWidget(WDFunc::NewLBL(parent, "Частота:"));
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "value2", ValuesFormat, "Частота, Гц"));
    lyout->addLayout(hlyout);
    for (i = 0; i < 3; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "Ueff"+IndexStr),0,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+3), ValuesFormat, \
                                          QString::number(i+3)+".Действующие значения напряжений по 1-й гармонике, кВ"),1,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Ieff"+IndexStr),2,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+6), ValuesFormat, \
                                          QString::number(i+6)+".Действующие значения токов по 1-й гармонике, мА"),3,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Cbush"+IndexStr),4,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+9), ValuesFormat, \
                                          QString::number(i+9)+".Ёмкости вводов, пФ"),5,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Tg_d"+IndexStr),6,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+12), ValuesFormat, \
                                          QString::number(i+12)+".Ёмкости вводов, пФ"),7,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "dCbush"+IndexStr),8,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+15), ValuesFormat, \
                                          QString::number(i+15)+".Изменение емкостей вводов, пФ"),9,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "dTg_d"+IndexStr),10,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+18), ValuesFormat, \
                                          QString::number(i+18)+".Изменение тангенсов дельта вводов, %"),11,i,1,1);
    }


    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *Check_84::Bd2W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "U0"),0,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value21", ValuesFormat, \
                                      QString::number(21)+".Напряжение нулевой последовательности"),1,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U1"),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value22", ValuesFormat, \
                                      QString::number(22)+".Напряжение прямой последовательности"),1,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U2"),0,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value23", ValuesFormat, \
                                      QString::number(23)+".Напряжение обратной последовательности"),1,2,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I0"),2,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value24", ValuesFormat, \
                                      QString::number(24)+".Ток нулевой последовательности"),3,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I1"),2,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value25", ValuesFormat, \
                                      QString::number(25)+".Ток прямой последовательности"),3,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I2"),2,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value26", ValuesFormat, \
                                      QString::number(26)+".Ток обратной последовательности"),3,2,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Iunb"),4,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value27", ValuesFormat, \
                                      QString::number(27)+".Ток обратной последовательности"),5,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Phy_unb"),4,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value28", ValuesFormat, \
                                      QString::number(28)+".Угол тока небаланса относительно тока ф.А, град."),5,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Time"),4,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value29", ValuesFormat, \
                                      QString::number(29)+".Время записи, с"),5,2,1,1);



    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;

}

QWidget *Check_84::Bd3W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    if (ModuleBSI::GetMType(BoardTypes::BT_BASE) != Config::MTB_A2)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "Tamb"),6,0,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value30", ValuesFormat, \
                                          QString::number(30)+".Температура окружающей среды, °С"),7,0,1,1);
    }


    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}


void Check_84::FillBd(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "value0", WDFunc::StringValueWithCheck(Bd_block1.NUM, 3));
    WDFunc::SetLBLText(parent, "value1", WDFunc::StringValueWithCheck(Bd_block1.Tmk, 3));
    WDFunc::SetLBLText(parent, "value2", WDFunc::StringValueWithCheck(Bd_block1.Frequency, 3));
    for (int i = 0; i < 3; i++)
    {

        WDFunc::SetLBLText(parent, "value"+QString::number(i+3), WDFunc::StringValueWithCheck(Bd_block1.Ueff[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+6), WDFunc::StringValueWithCheck(Bd_block1.Ieff[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+9), WDFunc::StringValueWithCheck(Bd_block1.Cbush[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+12), WDFunc::StringValueWithCheck(Bd_block1.Tg_d[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+15), WDFunc::StringValueWithCheck(Bd_block1.dCbush[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+18), WDFunc::StringValueWithCheck(Bd_block1.dTg_d[i], 3));
    }

    WDFunc::SetLBLText(parent, "value21", WDFunc::StringValueWithCheck(Bd_block1.U0, 3));
    WDFunc::SetLBLText(parent, "value22", WDFunc::StringValueWithCheck(Bd_block1.U1, 3));
    WDFunc::SetLBLText(parent, "value23", WDFunc::StringValueWithCheck(Bd_block1.U2, 3));
    WDFunc::SetLBLText(parent, "value24", WDFunc::StringValueWithCheck(Bd_block1.I0, 3));
    WDFunc::SetLBLText(parent, "value25", WDFunc::StringValueWithCheck(Bd_block1.I1, 3));
    WDFunc::SetLBLText(parent, "value26", WDFunc::StringValueWithCheck(Bd_block1.I2, 3));
    WDFunc::SetLBLText(parent, "value27", WDFunc::StringValueWithCheck(Bd_block1.Iunb, 3));
    WDFunc::SetLBLText(parent, "value28", WDFunc::StringValueWithCheck(Bd_block1.Phy_unb, 3));
    WDFunc::SetLBLText(parent, "value29", WDFunc::StringValueWithCheck(Bd_block1.Time, 3));


    if (ModuleBSI::GetMType(BoardTypes::BT_BASE) != Config::MTB_A2)
    {
        WDFunc::SetLBLText(parent, "value30", WDFunc::StringValueWithCheck(Bd_block1.Tamb, 3));
    }

}

/*void Check_84::FillBd2(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "value21", WDFunc::StringValueWithCheck(Bd_block1.U0, 3));
    WDFunc::SetLBLText(parent, "value22", WDFunc::StringValueWithCheck(Bd_block1.U1, 3));
    WDFunc::SetLBLText(parent, "value23", WDFunc::StringValueWithCheck(Bd_block1.U2, 3));
    WDFunc::SetLBLText(parent, "value24", WDFunc::StringValueWithCheck(Bd_block1.I0, 3));
    WDFunc::SetLBLText(parent, "value25", WDFunc::StringValueWithCheck(Bd_block1.I1, 3));
    WDFunc::SetLBLText(parent, "value26", WDFunc::StringValueWithCheck(Bd_block1.I2, 3));
    WDFunc::SetLBLText(parent, "value27", WDFunc::StringValueWithCheck(Bd_block1.Iunb, 3));
    WDFunc::SetLBLText(parent, "value28", WDFunc::StringValueWithCheck(Bd_block1.Phy_unb, 3));
    WDFunc::SetLBLText(parent, "value29", WDFunc::StringValueWithCheck(Bd_block1.Time, 3));


    if (ModuleBSI::GetMType(BoardTypes::BT_BASE) != Config::MTB_A2)
    {
        WDFunc::SetLBLText(parent, "value30", WDFunc::StringValueWithCheck(Bd_block1.Tamb, 3));
    }
}*/
