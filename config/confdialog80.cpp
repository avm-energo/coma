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
#include "confdialog80.h"

ConfDialog80::ConfDialog80(QVector<S2::DataRec> &S2Config, QWidget *parent) :
    AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    this->S2Config = &S2Config;
    C80 = new Config80(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);

    SetupUI();
    PrereadConf();
}

ConfDialog80::~ConfDialog80()
{
}

void ConfDialog80::Fill()
{
    int i;
    WDFunc::SetCBIndex(this, "eq_typecb", C80->Bci_block.eq_type);
    WDFunc::SetChBData(this, "oscchb.0", (C80->Bci_block.ddosc & 0x0001));
    WDFunc::SetChBData(this, "oscchb.1", (C80->Bci_block.ddosc & 0x0002));
    WDFunc::SetChBData(this, "oscchb.2", (C80->Bci_block.ddosc & 0x0004));
    WDFunc::SetSPBData(this, "npointspb", C80->Bci_block.npoints);
    WDFunc::SetSPBData(this, "nfiltrspb", C80->Bci_block.nfiltr);
    WDFunc::SetSPBData(this, "nhfiltrspb", C80->Bci_block.nhfiltr);
    switch (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN))
    {
    case MTM_81: // 2 напряжения, 0 токов
    {
        WDFunc::SetCBData(this, "unom.1", QString::number(C80->Bci_block.unom1));
        WDFunc::SetCBData(this, "unom.2", QString::number(C80->Bci_block.unom2));
        WDFunc::SetSPBData(this, "thr.1", C80->Bci_block.duosc);
       break;
    }
    case MTM_82:
    {
        WDFunc::SetCBData(this, "unom.1", QString::number(C80->Bci_block.unom1));
        for (i = 3; i < 6; i++)
            WDFunc::SetSPBData(this, "inom.2."+QString::number(i), C80->Bci_block.inom2[i]);
        WDFunc::SetSPBData(this, "thr.1", C80->Bci_block.duosc);
        WDFunc::SetSPBData(this, "thr.2", C80->Bci_block.diosc);
        break;
    }
    case MTM_83:
    {
        for (i = 0; i < 6; i++)
        {
            WDFunc::SetSPBData(this, "inom.1."+QString::number(i), C80->Bci_block.inom1[i]);
            WDFunc::SetSPBData(this, "inom.2."+QString::number(i), C80->Bci_block.inom2[i]);
        }
        WDFunc::SetSPBData(this, "thr.2", C80->Bci_block.diosc);
       break;
    }
    default:
        break;
    }
    WDFunc::SetSPBData(this, "thr.3", C80->Bci_block.duimin);
}

void ConfDialog80::FillBack()
{
    int i;
    QString tmps;
    bool tmpb;
    WDFunc::CBIndex(this, "eq_typecb", C80->Bci_block.eq_type);
    WDFunc::ChBData(this, "oscchb.0", tmpb);
    C80->Bci_block.ddosc |= (tmpb) ? 0x0001 : 0x0000;
    WDFunc::ChBData(this, "oscchb.1", tmpb);
    C80->Bci_block.ddosc |= (tmpb) ? 0x0002 : 0x0000;
    WDFunc::ChBData(this, "oscchb.2", tmpb);
    C80->Bci_block.ddosc |= (tmpb) ? 0x0004 : 0x0000;
    WDFunc::SPBData(this, "npointspb", C80->Bci_block.npoints);
    WDFunc::SPBData(this, "nfiltrspb", C80->Bci_block.nfiltr);
    WDFunc::SPBData(this, "nhfiltrspb", C80->Bci_block.nhfiltr);
    switch (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN))
    {
    case MTM_81: // 2 напряжения, 0 токов
    {
        WDFunc::CBData(this, "unom.1", tmps);
        C80->Bci_block.unom1 = tmps.toFloat();
        WDFunc::CBData(this, "unom.2", tmps);
        C80->Bci_block.unom2 = tmps.toFloat();
        WDFunc::SPBData(this, "thr.1", C80->Bci_block.duosc);
       break;
    }
    case MTM_82:
    {
        WDFunc::CBData(this, "unom.1", tmps);
        C80->Bci_block.unom1 = tmps.toFloat();
        for (i = 3; i < 6; i++)
        {
            WDFunc::SPBData(this, "inom.1."+QString::number(i), C80->Bci_block.inom1[i]);
            WDFunc::SPBData(this, "inom.2."+QString::number(i), C80->Bci_block.inom2[i]);
        }
        WDFunc::SPBData(this, "thr.1", C80->Bci_block.duosc);
        WDFunc::SPBData(this, "thr.2", C80->Bci_block.diosc);
        break;
    }
    case MTM_83:
    {
        for (i = 0; i < 6; i++)
        {
            WDFunc::SPBData(this, "inom.1."+QString::number(i), C80->Bci_block.inom1[i]);
            WDFunc::SPBData(this, "inom.2."+QString::number(i), C80->Bci_block.inom2[i]);
        }
        WDFunc::SPBData(this, "thr.2", C80->Bci_block.diosc);
       break;
    }
    default:
        break;
    }
    WDFunc::SPBData(this, "thr.3", C80->Bci_block.duimin);
}

void ConfDialog80::SetupUI()
{
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QWidget *cp1 = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);

    hlyout->addWidget(WDFunc::NewLBL(this, "Тип контролируемого оборудования:"), 0);
    QStringList cbl = QStringList() << "1ф трансформатор/АТ" << "3ф трансформатор/АТ" << "1ф реактор" << "3ф реактор";
    EComboBox *cb = WDFunc::NewCB(this, "eq_typecb", cbl, ACONFGCLR);
    cb->setMinimumWidth(70);
    hlyout->addWidget(cb,10);
    vlyout1->addLayout(hlyout);

    QGroupBox *gb = new QGroupBox("Аналоговые");
    switch (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN))
    {
    case MTM_81: // 6U0I
    {
        vlyout2->addWidget(UNom(1));
        vlyout2->addWidget(UNom(2));
        vlyout2->addWidget(Threshold("Уставка скачка напряжения для запуска осциллографирования, %", 1));
        break;
    }
    case MTM_82: // 3U3I
    {
        vlyout2->addWidget(UNom(1));
        vlyout2->addWidget(INom(3), 10);
        vlyout2->addWidget(INom(4), 10);
        vlyout2->addWidget(Threshold("Уставка скачка напряжения для запуска осциллографирования, %", 1));
        vlyout2->addWidget(Threshold("Уставка скачка тока для запуска осциллографирования, %", 2));
        break;
    }
    case MTM_83: // 0U6I
    {
        vlyout2->addWidget(INom(1), 10);
        vlyout2->addWidget(INom(2), 10);
        vlyout2->addWidget(INom(3), 10);
        vlyout2->addWidget(INom(4), 10);
        vlyout2->addWidget(Threshold("Уставка скачка тока для запуска осциллографирования, %", 2));
        break;
    }
    default:
        break;
    }
    vlyout2->addWidget(Threshold("Уставка порога мин. уровня для определения частоты, %", 3));
    vlyout2->addWidget(new QPushButton("123"));
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

    gb = new QGroupBox("Прочие");
    vlyout2 = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Кол-во точек оцифровки:"));
    cbl = QStringList() << "64" << "80" << "128" << "256";
    hlyout->addWidget(WDFunc::NewCB(this, "npointscb", cbl, ACONFWCLR));
    vlyout2->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Постоянная времени фильтрации:"));
    hlyout->addWidget(WDFunc::NewSPB(this, "nfiltrspb", 1, 1000, 0, ACONFWCLR));
    vlyout2->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Постоянная времени гармоник:"));
    hlyout->addWidget(WDFunc::NewSPB(this, "nhfiltrspb", 1, 100, 0, ACONFWCLR));
    vlyout2->addLayout(hlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    cp1->setLayout(vlyout1);

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(cp1,"Общие");
    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void ConfDialog80::CheckConf()
{
}

QWidget *ConfDialog80::UNom(int numunom)
{
    QWidget *w = new QWidget(this);
    QString NumUNomStr = QString::number(numunom);
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QLabel *lbl=new QLabel("Класс напряжения "+NumUNomStr+"-й группы, кВ:");
    gb2lyout->addWidget(lbl);
    QStringList cbl = QStringList() << "1150" << "750" << "500" << "330" << "220" << "110" << "35" << "21" << "15.75" << "11" << "10" << "6.3";
    EComboBox *cb = WDFunc::NewCB(this, "unom."+NumUNomStr, cbl, ACONFWCLR);
    cb->setEditable(true);
    gb2lyout->addWidget(cb);
    w->setLayout(gb2lyout);
    return w;
}

// 1 - первичный ток первой группы, 2 - вторичный ток первой группы, 3,4 - то же по второй группе

QWidget *ConfDialog80::INom(int numinom)
{
    QWidget *w = new QWidget;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    int Group = (numinom < 3) ? 0 : 1;
    QString Perv = (numinom%2) ? "первичные" : "вторичные";
    QString PervNum = (numinom%2) ? "1" : "2";
    gb2lyout->addWidget(WDFunc::NewLBL(this, "Номинальные "+Perv+" токи ТТ "+QString::number(Group+1)+"-й группы, А: "), 0);
    for (int i = 0; i < 3; i++)
    {
        gb2lyout->addWidget(WDFunc::NewLBL(this, QString::number(i+10, 16).toUpper() + ":"), 1, Qt::AlignRight); // A, B, C
        gb2lyout->addWidget(WDFunc::NewSPB(this, "inom."+PervNum+"."+QString::number(i+Group*3), 1, 50000, 0, ACONFWCLR), 1);
    }
    w->setLayout(gb2lyout);
    return w;
}

QWidget *ConfDialog80::Threshold(QString str, int numthr)
{
    QWidget *w = new QWidget;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    gb2lyout->addWidget(WDFunc::NewLBL(this, str));
    gb2lyout->addWidget(WDFunc::NewSPB(this, "thr."+QString::number(numthr), 0, 100, 1, ACONFWCLR));
    w->setLayout(gb2lyout);
    return w;
}

void ConfDialog80::SetDefConf()
{
    C80->SetDefConf();
}
