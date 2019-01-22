#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include "../widgets/emessagebox.h"
#include "../widgets/ecombobox.h"
#include "../widgets/wd_func.h"
//#include "../gen/publicclass.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "confdialog84.h"


ConfDialog84::ConfDialog84(QVector<S2::DataRec> &S2Config, QWidget *parent) :
    AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    this->S2Config = &S2Config;
    C84 = new Config84(S2Config);
     setAttribute(Qt::WA_DeleteOnClose);

    SetupUI();
#if PROGSIZE != PROGSIZE_EMUL
    PrereadConf();
#endif
}

ConfDialog84::~ConfDialog84()
{
}

void ConfDialog84::Fill()
{

}

void ConfDialog84::FillBack()
{
    int i,index;
    QString tmps;
    bool tmpb;

}

void ConfDialog84::SetupUI()
{
    QString phase[3] = {"A:","B:","C:"};
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QWidget *analog1 = new QWidget;
    QWidget *analog2 = new QWidget;
    QWidget *extraconf = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    analog1->setStyleSheet(tmps);
    analog2->setStyleSheet(tmps);
    extraconf->setStyleSheet(tmps);

    QString paramcolor = MAINWINCLR;
    int row = 0;
    QGroupBox *gb = new QGroupBox("Аналоговые параметры");

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное линейное первичное напряжение, кВ:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Unom", 1, 1000, 0, paramcolor), row,2,1,3);
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума напряжения (в % от номинального):"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Umin", 1, 1000, 0, paramcolor), row,2,1,3);
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума тока (в % от Imax):"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Imin", 1, 1000, 0, paramcolor), row,2,1,3);
    row++;

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewLBL(this, phase[i]), row,2+i,1,1,Qt::AlignLeft);
    }
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Максимальные действующие значение токов вводов, мА:"), row,1,1,1);
    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "Imax."+QString::number(i), 1, 1000, 0, paramcolor), row,2+i,1,1);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения емкостей вводов:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "C_init."+QString::number(i), 1, 1000, 0, paramcolor), row,2+i,1,1);
    }
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения tg δ вводов:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "Tg_init."+QString::number(i), 1, 1000, 0, paramcolor), row,2+i,1,1);
    }
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Коррекция  tg δ вводов:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "corTg ."+QString::number(i), 1, 1000, 0, paramcolor), row, 2+i, 1, 1);
    }
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис на отключение сигнализации по dC, % от уставки:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "GdС", 1, 1000, 0, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис на отключение сигнализации по dTg, % от уставки:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "GdTg", 1, 1000, 0, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис на отключение сигнализации по небалансу токов:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "GdIunb", 1, 1000, 0, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное действ. значение тока небаланса:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Iunb_init", 1, 1000, 0, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное значение угла тока небаланса:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Phy_unb_init", 1, 1000, 0, paramcolor), row,2,1,3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    /*gb = new QGroupBox("Осциллограммы");
    vlyout2 = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Запуск осциллограммы:"));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.0", "по команде Ц", ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.1", "по дискр. входу PD1", ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.2", "по резкому изменению", ACONFWCLR));
    vlyout2->addLayout(hlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);*/



    analog1->setLayout(vlyout1);

    gb = new QGroupBox("Дополнительные");
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    vlyout2->addWidget(WDFunc::NewLBL(this, "Номинальные токи ТТ "+QString::number(1)+"-й группы, А: "), 0);
    for (int i = 0; i < 3; i++)
    {
        vlyout2->addWidget(WDFunc::NewLBL(this, QString::number(i+10, 16).toUpper() + ":"), 1, Qt::AlignLeft); // A, B, C
        vlyout2->addWidget(WDFunc::NewSPB(this, "inom."+QString::number(i), 1, 50000, 0, ACONFWCLR), 1);
        //hlyout->addStretch(10);
    }
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Осциллограммы");
    vlyout2 = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Запуск осциллограммы:"));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.0", "по команде Ц", ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.1", "по дискр. входу PD1", ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.2", "по резкому изменению", ACONFWCLR));
    vlyout2->addLayout(hlyout);


    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    analog2->setLayout(vlyout2);

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(analog1,"Аналоговые");
    ConfTW->addTab(analog2,"Уставки");

    if(ModuleBSI::GetMType(BoardTypes::BT_BASE) == Config::MTB_A2)
    ConfTW->addTab(extraconf,"Связь");

    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void ConfDialog84::CheckConf()
{
}




void ConfDialog84::SetDefConf()
{
    C84->SetDefConf();
}
