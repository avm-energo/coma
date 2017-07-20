#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include "../canal.h"
#include "../widgets/messagebox.h"
#include "../widgets/s_tqcombobox.h"
#include "../widgets/s_tqcheckbox.h"
#include "../widgets/s_tqspinbox.h"
#include "../widgets/wd_func.h"
#include "confdialoga1.h"

ConfDialogA1::ConfDialogA1(QVector<publicclass::DataRec> &S2Config, QWidget *parent) :
    AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    this->S2Config = &S2Config;
    CA1 = new ConfigA1(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);

    SetupUI();
    PrereadConf();
}

ConfDialogA1::~ConfDialogA1()
{
}

void ConfDialogA1::Fill()
{
    WDFunc::SetLEData(this, "DividerSN", QString::number(CA1->Bci_block.DNFNum));
    WDFunc::SetSPBData(this, "K_DNSPB", CA1->Bci_block.K_DN);
    WDFunc::SetCBIndex(this, "TokrCB", CA1->Bci_block.DTCanal);
    WDFunc::SetSPBData(this, "TokrSPB4", CA1->Bci_block.T4);
    WDFunc::SetSPBData(this, "TokrSPB20", CA1->Bci_block.T20);
    WDFunc::SetSPBData(this, "TokrW100", CA1->Bci_block.W100);
    WDFunc::SetCBIndex(this, "HumidityCB", (CA1->Bci_block.DHCanal > 0) ? (CA1->Bci_block.DHCanal-1) : 0);
    WDFunc::SetSPBData(this, "HumiditySPB4", CA1->Bci_block.H4);
    WDFunc::SetSPBData(this, "HumiditySPB20", CA1->Bci_block.H20);
    WDFunc::SetSPBData(this, "thr.1", CA1->Bci_block.DUosc);
    WDFunc::SetSPBData(this, "thr.2", CA1->Bci_block.DUmin);
    WDFunc::SetCBData(this, "NPointsCB", QString::number(CA1->Bci_block.NPoints));
    WDFunc::SetSPBData(this, "NFilterSPB", CA1->Bci_block.Filter);
    WDFunc::SetSPBData(this, "NHarmFiltSPB", CA1->Bci_block.NHarmFilt);
}

void ConfDialogA1::FillBack()
{
    QString tmps;
    int tmpi;
    CA1->Bci_block.DDOsc = 0;
    WDFunc::LEData(this, "DividerSN", tmps);
    CA1->Bci_block.DNFNum = tmps.toInt();
    WDFunc::SPBData(this, "K_DNSPB", CA1->Bci_block.K_DN);
    WDFunc::CBIndex(this, "TokrCB", CA1->Bci_block.DTCanal);
    WDFunc::SPBData(this, "TokrSPB4", CA1->Bci_block.T4);
    WDFunc::SPBData(this, "TokrSPB20", CA1->Bci_block.T20);
    WDFunc::SPBData(this, "TokrW100", CA1->Bci_block.W100);
    WDFunc::CBIndex(this, "HumidityCB", tmpi);
    CA1->Bci_block.DHCanal = (tmpi > 0) ? (tmpi+1) : 0;
    WDFunc::SPBData(this, "HumiditySPB4", CA1->Bci_block.H4);
    WDFunc::SPBData(this, "HumiditySPB20", CA1->Bci_block.H20);
    WDFunc::SPBData(this, "thr.1", CA1->Bci_block.DUosc);
    WDFunc::SPBData(this, "thr.2", CA1->Bci_block.DUmin);
    WDFunc::CBData(this, "NPointsCB", tmps);
    CA1->Bci_block.NPoints = tmps.toInt();
    WDFunc::SPBData(this, "NFilterSPB", CA1->Bci_block.Filter);
    WDFunc::SPBData(this, "NHarmFiltSPB", CA1->Bci_block.NHarmFilt);
}

void ConfDialogA1::SetupUI()
{
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QWidget *cp1 = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);

    hlyout->addWidget(WDFunc::NewLBL(this,"Заводской номер делителя:"), 0);
    hlyout->addWidget(WDFunc::NewLE(this, "DividerSN", "00000000", tmps),10);
    hlyout->addWidget(WDFunc::NewLBL(this,"Номинальный коэффициент деления ДН:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this,"K_DNSPB",1,10000,1,0,ACONFWCLR));
    hlyout->addStretch(10);
    vlyout1->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Канал датчика Tокр:"));
    QStringList sl = QStringList() << "0. Нет датчика" << "1. Канал Pt100" << "2. Канал А1" << "3. Канал А2";
    s_tqComboBox *cb = WDFunc::NewCB(this, "TokrCB", sl, ACONFGCLR);
    hlyout->addWidget(cb, 10);
    hlyout->addWidget(WDFunc::NewLBL(this, "W100:"));
    hlyout->addWidget(WDFunc::NewSPB(this, "TokrW100", 0.1, 3, 0.001, 3, ACONFGCLR));
    hlyout->addStretch(10);
    vlyout1->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Значение при 4 мА:"), 0, Qt::AlignRight);
    hlyout->addWidget(WDFunc::NewSPB(this, "TokrSPB4", 0, 100, 0.1, 1, ACONFGCLR), 10);
    hlyout->addWidget(WDFunc::NewLBL(this, "Значение при 20 мА:"), 0, Qt::AlignRight);
    hlyout->addWidget(WDFunc::NewSPB(this, "TokrSPB20", 0, 100, 0.1, 1, ACONFGCLR), 10);
    hlyout->addStretch(10);
    vlyout1->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Канал датчика влажности:"));
    sl = QStringList() << "0. Нет датчика" << "2. Канал А1" << "3. Канал А2";
    cb = WDFunc::NewCB(this, "HumidityCB", sl, ACONFGCLR);
    hlyout->addWidget(cb);
    hlyout->addStretch(10);
    vlyout1->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Значение при 4 мА:"), 0, Qt::AlignRight);
    hlyout->addWidget(WDFunc::NewSPB(this, "HumiditySPB4", 0, 100, 0.1, 1, ACONFGCLR));
    hlyout->addWidget(WDFunc::NewLBL(this, "Значение при 20 мА:"), 0, Qt::AlignRight);
    hlyout->addWidget(WDFunc::NewSPB(this, "HumiditySPB20", 0, 100, 0.1, 1, ACONFGCLR));
    hlyout->addStretch(10);
    vlyout1->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Уставка скачка напряжения для запуска осциллографирования, %"));
    hlyout->addWidget(WDFunc::NewSPB(this, "thr.1", 0, 100, 0.1, 1, ACONFWCLR));
    hlyout->addStretch(10);
    vlyout1->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума напряжения для частоты и фазового сдвига, %"));
    hlyout->addWidget(WDFunc::NewSPB(this, "thr.2", 0, 100, 0.1, 1, ACONFWCLR));
    hlyout->addStretch(10);
    vlyout1->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Количество точек оцифровки на период:"), 0);
    sl = QStringList() << "64" << "128" << "256";
    cb = WDFunc::NewCB(this, "NPointsCB", sl, ACONFGCLR);
    hlyout->addWidget(cb, 10);
    hlyout->addStretch(10);
    vlyout1->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLT(this, "Постоянная фильтрации:", "", "", "Задаётся в тактах, один такт - 20 мс"));
    hlyout->addWidget(WDFunc::NewSPB(this, "NFilterSPB", 1, 1000, 1, 0, ACONFGCLR));
    hlyout->addStretch(10);
    vlyout1->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLT(this, "Постоянная фильтрации гармоник:", "", "", "Задаётся в циклах, каждый цикл равен количеству гармоник, \n"
                                                                                       "умноженному на длину одного такта (20 мс), по умолчанию 61 такт"));
    hlyout->addWidget(WDFunc::NewSPB(this, "NHarmFiltSPB", 1, 1000, 1, 0, ACONFGCLR));
    hlyout->addStretch(10);
    vlyout1->addLayout(hlyout);

    cp1->setLayout(vlyout1);

    vlyout1 = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(cp1,"Конфигурация");
    vlyout1->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    vlyout1->addWidget(wdgt);
    setLayout(vlyout1);
}

void ConfDialogA1::CheckConf()
{
    CheckConfErrors.clear();
}

void ConfDialogA1::SetDefConf()
{
    CA1->SetDefConf();
}