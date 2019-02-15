#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QTabBar>
#include "confdialog87.h"
#include "../gen/stdfunc.h"
#include "../gen/maindef.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

ConfDialog87::ConfDialog87(QVector<S2::DataRec> &S2Config, bool BaseBoard, QWidget *parent) :
    AbstractConfDialog(parent)
{
    Q_UNUSED(BaseBoard);
    //RangeInMins = {4.0, 0.0, 0.0, -5.0, 0.0};
    //RangeInMaxs = {20.0, 20.0, 5.0, 5.0, 5.0};
    this->S2Config = &S2Config;
    C87 = new Config87(S2Config);
    Params.InTypes = QStringList() << "Не исп." << "мА";
    //Params.RangeTypes = QStringList() << "(4..20) мА" << "(0..20) мА" << "(0..5) мА" << \
    //                                     "(-5..5) В" << "(0..5) В" << "Произвольный мА" << "Произвольный В";
    Params.NumCh = CHR87_NUMCH;
    SetupUI();

    #if PROGSIZE != PROGSIZE_EMUL
        PrereadConf();
    #endif
}

void ConfDialog87::SetupUI()
{
    int i;
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    QVBoxLayout *lyout = new QVBoxLayout;

    QGroupBox *gb = new QGroupBox("Типы каналов");
    QVBoxLayout *gblyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    for (i = 0; i < Params.NumCh; ++i)
    {
        QLabel *ChTypL = new QLabel(QString::number(i+1)+":");
        EComboBox *cb = WDFunc::NewCB(this, "chtypcb."+QString::number(i), Params.InTypes, ACONFWCLR);
        connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetChTypData(int)));
        hlyout->addWidget(ChTypL);
        hlyout->addWidget(cb, 1);
        if (((i+1) % 4) == 0)
        {
            gblyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    gb->setLayout(gblyout);
    lyout->addWidget(gb);

    gblyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    gblyout = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    hlyout->addStretch(10);

    QWidget *cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFGCLR)+";}";
    cp->setStyleSheet(tmps);
    cp->setLayout(lyout);
    ConfTW->addTab(cp,"Общие");

    lyout = new QVBoxLayout;
    gb = new QGroupBox("Параметры");
    gb->setObjectName("RangeGB");

    int row;

    glyout = new QGridLayout;
    glyout->setColumnStretch(0,0);
    glyout->setColumnStretch(1,10);
    glyout->setColumnStretch(2,5);
    glyout->setColumnStretch(3,5);
    glyout->setColumnStretch(4,5);
    glyout->setColumnStretch(5,5);
    glyout->addWidget(WDFunc::NewLBL(this, "№ канала"),0,0,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBLT(this, "Num points", "", tmps, "Количество точек выше порога, %"),0,1,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBLT(this, "T down", "", tmps,"Нижняя граница длительности сигнала, мкс"),0,2,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBLT(this, "T up", "", tmps,"Верхняя граница длительности сигнала, мкс"),0,3,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBLT(this, "F down", "", tmps,"Нижняя частотная граница сигнала, Гц"),0,4,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBLT(this, "F up", "", tmps,"Верхняя частотная граница сигнала, Гц"),0,5,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBLT(this, "A down", "", tmps,"Уровень низкой степени опасности, мВ"),0,6,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBLT(this, "A up", "", tmps,"Уровень высокой степени опасности, мВ"),0,7,1,1,Qt::AlignCenter);
    for (i = 0; i < CHR87_NUMCH; ++i)
    {
        row = i+1;
        glyout->addWidget(WDFunc::NewLBL(this, QString::number(i+1)),row,0,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "percentLimit"+QString::number(i), -100000.0, 100000.0, 2, ACONFYCLR),row,1,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "Tdown"+QString::number(i), -100000.0, 100000.0, 2, ACONFYCLR),row,2,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "Tup"+QString::number(i), -100000.0, 100000.0, 2, ACONFRCLR),row,3,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "Fdown"+QString::number(i), -100000.0, 100000.0, 2, ACONFYCLR),row,4,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "Fup"+QString::number(i), -100000.0, 100000.0, 2, ACONFRCLR),row,5,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "Adown"+QString::number(i), -100000.0, 100000.0, 2, ACONFYCLR),row,6,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "Aup"+QString::number(i), -100000.0, 100000.0, 2, ACONFRCLR),row,7,1,1);
    }

    /*glyout->addWidget(WDFunc::NewLBL(this, "Количество точек выше порога, %:"),0,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "percentLimit", 0, 50000, 0,MAINWINCLR),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Нижняя граница длительности сигнала, мкс"),1,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Tdown", 0, 50000, 1,MAINWINCLR),1,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Верхняя граница длительности сигнала, мкс"),2,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Tup", 0, 50000, 1,MAINWINCLR),2,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Нижняя частотная граница сигнала, Гц"),3,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Fdown", 0, 50000, 1,MAINWINCLR),3,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Верхняя частотная граница сигнала, Гц"),4,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Fup", 0, 50000, 1,MAINWINCLR),4,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Уровень низкой степени опасности, мВ"),5,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Adown", 0, 50000, 1,MAINWINCLR),5,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Уровень высокой степени опасности, мВ"),6,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Aup", 0, 50000, 1,MAINWINCLR),6,1,1,1);*/
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFGCLR)+";}";
    cp->setStyleSheet(tmps);
    cp->setLayout(lyout);
    ConfTW->addTab(cp,"Параметры ЧР");



    lyout = new QVBoxLayout;
    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void ConfDialog87::DisableChannel(int chnum, bool disable)
{
    Q_UNUSED(chnum);
    Q_UNUSED(disable);
}

void ConfDialog87::SetRange(int RangeType)
{
    Q_UNUSED(RangeType);
}

void ConfDialog87::Fill()
{
    for (int i = 0; i < CHR87_NUMCH; i++)
    {
        WDFunc::SetSPBData(this, "percentLimit"+QString::number(i), C87->Bci_block.percentLimit[i]);
        WDFunc::SetSPBData(this, "Tdown"+QString::number(i), C87->Bci_block.T_down[i]);
        WDFunc::SetSPBData(this, "Tup"+QString::number(i), C87->Bci_block.T_up[i]);
        WDFunc::SetSPBData(this, "Fdown"+QString::number(i), C87->Bci_block.F_down[i]);
        WDFunc::SetSPBData(this, "Fup"+QString::number(i), C87->Bci_block.F_up[i]);
        WDFunc::SetSPBData(this, "Adown"+QString::number(i), C87->Bci_block.A_down[i]);
        WDFunc::SetSPBData(this, "Aup"+QString::number(i), C87->Bci_block.A_up[i]);
    }

}

void ConfDialog87::FillBack()
{
    for (int i = 0; i < CHR87_NUMCH; i++)
    {
        WDFunc::SetSPBData(this, "percentLimit"+QString::number(i), C87->Bci_block.percentLimit[i]);
        WDFunc::SetSPBData(this, "Tdown"+QString::number(i), C87->Bci_block.T_down[i]);
        WDFunc::SetSPBData(this, "Tup"+QString::number(i), C87->Bci_block.T_up[i]);
        WDFunc::SetSPBData(this, "Fdown"+QString::number(i), C87->Bci_block.F_down[i]);
        WDFunc::SetSPBData(this, "Fup"+QString::number(i), C87->Bci_block.F_up[i]);
        WDFunc::SetSPBData(this, "Adown"+QString::number(i), C87->Bci_block.A_down[i]);
        WDFunc::SetSPBData(this, "Aup"+QString::number(i), C87->Bci_block.A_up[i]);

    }
}

void ConfDialog87::SetMinMax(int i)
{
    Q_UNUSED(i);
}

void ConfDialog87::SetChTypData(int value)
{
    Q_UNUSED(value);
}

void ConfDialog87::CheckConf()
{

}

void ConfDialog87::SetDefConf()
{
    C87->SetDefConf();
    Fill();
}
