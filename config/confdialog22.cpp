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
#include <QDoubleSpinBox>
#include <QTabBar>
#include "confdialog22.h"
#include "../widgets/emessagebox.h"
#include "../gen/publicclass.h"
#include "../widgets/wd_func.h"

ConfDialog22::ConfDialog22(QVector<S2::DataRec> &S2Config, bool BaseBoard, QWidget *parent) :
    AbstractConfDialog(parent)
{
    this->S2Config = &S2Config;
    C22 = new Config22(S2Config, BaseBoard);
    SetupUI();
    PrereadConf();
}

void ConfDialog22::SetupUI()
{
    int i;
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    QVBoxLayout *lyout = new QVBoxLayout;

    QGroupBox *gb = new QGroupBox("Свойства датчиков");
    QGridLayout *glyout = new QGridLayout;
    glyout->setColumnStretch(0,0);
    glyout->setColumnStretch(1,5);
    glyout->setColumnStretch(2,5);
    glyout->setColumnStretch(3,5);
    glyout->addWidget(WDFunc::NewLBL(this, "№ канала", ACONFWCLR),0,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(this, "R0", "", ACONFWCLRO, "Сопротивление при температуре 0 °C"),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(this, "W100", "", ACONFWCLRO, "Температурный коэффициент"),0,2,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Мин. °C", ACONFWCLRO),0,3,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Макс. °C", ACONFWCLR),0,4,1,1);
    for (i = 0; i < AIN22_NUMCH; ++i)
    {
        int column = 0;
        int row = i+1;
        glyout->addWidget(WDFunc::NewLBL(this, QString::number(i+1), ACONFWCLRO),row,column++,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "0."+QString::number(i), 20, 5000, 0, ACONFWCLR),row,column++,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "1."+QString::number(i), 0.001, 9.999, 3, ACONFWCLRO),row,column++,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "2."+QString::number(i), -1000.0, 1000.0, 0, ACONFWCLR),row,column++,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "3."+QString::number(i), -1000.0, 1000.0, 0, ACONFWCLR),row,column++,1,1);
    }
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLT(this, "kf_filtrT", "", "", "Пороговое значение, при превышении которого между замерами фильтр не работает"));
    hlyout->addWidget(WDFunc::NewSPB(this, "kf_filtrT", 0, 1000, 1, ACONFWCLRO));
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLT(this, "cnt_filtrT", "", "", "Степень фильтрации (0 - отсутствует)"));
    hlyout->addWidget(WDFunc::NewSPB(this, "cnt_filtrT", 0, 1000, 0, ACONFWCLR));
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLT(this, "hysteresisT", "", "", "Значение гистерезиса на сравнение с уставками (доля от диапазона)"));
    hlyout->addWidget(WDFunc::NewSPB(this, "hysteresisT", 0, 100, 1, ACONFWCLRO));
    lyout->addLayout(hlyout);
    QWidget *cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFGCLR)+";}";
    cp->setStyleSheet(tmps);
    cp->setLayout(lyout);
    ConfTW->addTab(cp,"Входы");

    lyout = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->setColumnStretch(0,0);
    glyout->setColumnStretch(1,5);
    glyout->setColumnStretch(2,5);
    glyout->setColumnStretch(3,5);
    glyout->setColumnStretch(4,5);
    glyout->addWidget(WDFunc::NewLBL(this, "№ канала"),0,0,1,1,Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBL(this, "Мин. авар., °C"),0,1,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Мин. пред., °C"),0,2,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Макс. пред., °C"),0,3,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Макс. авар., °C"),0,4,1,1,Qt::AlignCenter);
    for (i = 0; i < AIN22_NUMCH; ++i)
    {
        int column = 0;
        int row = i+1;
        glyout->addWidget(WDFunc::NewLBL(this, QString::number(i+1)),row,column++,1,1,Qt::AlignRight);
        glyout->addWidget(WDFunc::NewSPB(this, "4."+QString::number(i), -1000.0, 1000.0, 1, ACONFRCLR),row,column++,1,1,Qt::AlignCenter);
        glyout->addWidget(WDFunc::NewSPB(this, "5."+QString::number(i), -1000.0, 1000.0, 1, ACONFYCLR),row,column++,1,1,Qt::AlignCenter);
        glyout->addWidget(WDFunc::NewSPB(this, "6."+QString::number(i), -1000.0, 1000.0, 1, ACONFYCLR),row,column++,1,1,Qt::AlignCenter);
        glyout->addWidget(WDFunc::NewSPB(this, "7."+QString::number(i), -1000.0, 1000.0, 1, ACONFRCLR),row,column++,1,1,Qt::AlignCenter);
    }
    lyout->addLayout(glyout);
    cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp->setStyleSheet(tmps);
    cp->setLayout(lyout);
    ConfTW->addTab(cp,"Уставки");

    lyout = new QVBoxLayout;
    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void ConfDialog22::Fill()
{
    int i;
    WDFunc::SetSPBData(this, "kf_filtrT", C22->Bci_block.kf_filtrT);
    WDFunc::SetSPBData(this, "cnt_filtrT", C22->Bci_block.cnt_filtrT);
    WDFunc::SetSPBData(this, "hysteresisT", C22->Bci_block.hysteresisT);
    for (i = 0; i < AIN22_NUMCH; ++i)
    {
        WDFunc::SetSPBData(this, "0."+QString::number(i), C22->Bci_block.RzeroT[i]); // in_rzero
        WDFunc::SetSPBData(this, "1."+QString::number(i), C22->Bci_block.W100[i]); // in_w100
        WDFunc::SetSPBData(this, "2."+QString::number(i), C22->Bci_block.in_minT[i]); // in_min
        WDFunc::SetSPBData(this, "3."+QString::number(i), C22->Bci_block.in_maxT[i]); // in_max
        WDFunc::SetSPBData(this, "4."+QString::number(i), C22->Bci_block.setminminT[i]); // setminmin
        WDFunc::SetSPBData(this, "5."+QString::number(i), C22->Bci_block.setminT[i]); // setmin
        WDFunc::SetSPBData(this, "6."+QString::number(i), C22->Bci_block.setmaxT[i]); // setmax
        WDFunc::SetSPBData(this, "7."+QString::number(i), C22->Bci_block.setmaxmaxT[i]); // setmaxmax
    }
}

void ConfDialog22::FillBack()
{
    WDFunc::SPBData(this, "kf_filtrT", C22->Bci_block.kf_filtrT);
    WDFunc::SPBData(this, "cnt_filtrT", C22->Bci_block.cnt_filtrT);
    WDFunc::SPBData(this, "hysteresisT", C22->Bci_block.hysteresisT);
    for (int i=0; i<AIN22_NUMCH; ++i)
    {
        WDFunc::SPBData(this, "0."+QString::number(i), C22->Bci_block.RzeroT[i]); // in_rzero
        WDFunc::SPBData(this, "1."+QString::number(i), C22->Bci_block.W100[i]); // in_w100
        WDFunc::SPBData(this, "2."+QString::number(i), C22->Bci_block.in_minT[i]); // in_min
        WDFunc::SPBData(this, "3."+QString::number(i), C22->Bci_block.in_maxT[i]); // in_max
        WDFunc::SPBData(this, "4."+QString::number(i), C22->Bci_block.setminminT[i]); // setminmin
        WDFunc::SPBData(this, "5."+QString::number(i), C22->Bci_block.setminT[i]); // setmin
        WDFunc::SPBData(this, "6."+QString::number(i), C22->Bci_block.setmaxT[i]); // setmax
        WDFunc::SPBData(this, "7."+QString::number(i), C22->Bci_block.setmaxmaxT[i]); // setmaxmax
    }
}

void ConfDialog22::CheckConf()
{
    for (int i=0; i<AIN22_NUMCH; ++i)
    {
        if (C22->Bci_block.in_minT[i] > C22->Bci_block.in_maxT[i])
            CheckConfErrors.append("IN_MIN["+QString::number(i)+"] > IN_MAX["+QString::number(i)+"]");
        if (C22->Bci_block.setminminT[i] > C22->Bci_block.setminT[i])
            CheckConfErrors.append("SETMINMIN["+QString::number(i)+"] > SETMIN["+QString::number(i)+"]");
        if (C22->Bci_block.setmaxT[i] < C22->Bci_block.setminT[i])
            CheckConfErrors.append("SETMAX["+QString::number(i)+"] < SETMIN["+QString::number(i)+"]");
        if (C22->Bci_block.setmaxT[i] > C22->Bci_block.setmaxmaxT[i])
            CheckConfErrors.append("SETMAX["+QString::number(i)+"] > SETMAXMAX["+QString::number(i)+"]");
    }
}

void ConfDialog22::SetDefConf()
{
    C22->SetDefConf();
}
