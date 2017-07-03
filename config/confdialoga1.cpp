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
/*    int i;
    SetComboBox("eq_typecb", QString::number(econf->Bci_block.eq_type));
    SetCheckBox("oscchb.0", (econf->Bci_block.ddosc & 0x0001));
    SetCheckBox("oscchb.1", (econf->Bci_block.ddosc & 0x0002));
    SetCheckBox("oscchb.2", (econf->Bci_block.ddosc & 0x0004));
    SetSpinBox("npointspb", econf->Bci_block.npoints);
    SetSpinBox("nhfiltrspb", econf->Bci_block.nhfiltr);
    switch (pc.ModuleBsi.MTypeM)
    {
    case MTM_81: // 2 напряжения, 0 токов
    {
        SetComboBox("unom.1", QString::number(econf->Bci_block.unom1));
        SetComboBox("unom.2", QString::number(econf->Bci_block.unom2));
        SetSpinBox("thr.1", econf->Bci_block.duosc);
       break;
    }
    case MTM_82:
    {
        SetComboBox("unom.1", QString::number(econf->Bci_block.unom1));
        for (i = 3; i < 6; i++)
            SetSpinBox("inom.2."+QString::number(i), econf->Bci_block.inom2[i]);
        SetSpinBox("thr.1", econf->Bci_block.duosc);
        SetSpinBox("thr.2", econf->Bci_block.diosc);
        break;
    }
    case MTM_83:
    {
        for (i = 0; i < 6; i++)
        {
            SetSpinBox("inom.1."+QString::number(i), econf->Bci_block.inom1[i]);
            SetSpinBox("inom.2."+QString::number(i), econf->Bci_block.inom2[i]);
        }
        SetSpinBox("thr.2", econf->Bci_block.diosc);
       break;
    }
    default:
        break;
    }
    SetSpinBox("thr.3", econf->Bci_block.duimin);*/
}


bool ConfDialogA1::FillBack()
{

}

void ConfDialogA1::SetupUI()
{
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QWidget *cp1 = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);

    hlyout->addWidget(WDFunc::NewLBL(this,"Заводской номер делителя:"), 0);
    hlyout->addWidget(WDFunc::NewLE(this, "DividerSN", "00000000", ACONFGCLR),10);
    vlyout1->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this,"Класс напряжения делителя:"), 0);
    QStringList sl = QStringList() << "1150" << "750" << "500" << "330" << "220" << "110" << "35" << "21" << "15.75" << "11" << "10" << "6.3";
    QComboBox *cb = WDFunc::NewCB(this, "UnomCB", sl, ACONFWCLR);
    cb->setEditable(true);
    connect(cb,SIGNAL(textChanged(int)),this,SLOT(SetVoltageClass(int)));
    hlyout->addWidget(cb);
    vlyout1->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Канал датчика Tокр:"));
    sl = QStringList() << "0. Нет датчика" << "1. Канал Pt100" << "2. Канал А1" << "3. Канал А2";
    cb = WDFunc::NewCB(this, "TokrCB", sl, ACONFGCLR);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(TokrChoosed(int)));
    hlyout->addWidget(cb, 10);
    hlyout->addWidget(WDFunc::NewLBL(this, "Значение при 4 мА:"));
    hlyout->addWidget(WDFunc::NewSPB(this, "TokrSPB4", 0, 100, 0.1, 1, ACONFGCLR));
    hlyout->addWidget(WDFunc::NewLBL(this, "Значение при 20 мА:"));
    hlyout->addWidget(WDFunc::NewSPB(this, "TokrSPB20", 0, 100, 0.1, 1, ACONFGCLR));
    hlyout->addWidget(WDFunc::NewLBL(this, "W100:"));
    hlyout->addWidget(WDFunc::NewSPB(this, "TokrW100", 0.1, 3, 0.001, 3, ACONFGCLR));
    vlyout1->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Канал датчика влажности:"));
    sl = QStringList() << "0. Нет датчика" << "2. Канал А1" << "3. Канал А2";
    cb = WDFunc::NewCB(this, "HumidityCB", sl, ACONFGCLR);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(HumidityChoosed(int)));
    hlyout->addWidget(cb, 10);
    hlyout->addWidget(WDFunc::NewLBL(this, "Значение при 4 мА:"));
    hlyout->addWidget(WDFunc::NewSPB(this, "HumiditySPB4", 0, 100, 0.1, 1, ACONFGCLR));
    hlyout->addWidget(WDFunc::NewLBL(this, "Значение при 20 мА:"));
    hlyout->addWidget(WDFunc::NewSPB(this, "HumiditySPB20", 0, 100, 0.1, 1, ACONFGCLR));
    vlyout1->addLayout(hlyout);

    vlyout1->addWidget(Threshold("Уставка скачка напряжения для запуска осциллографирования, %", 1));
    vlyout1->addWidget(Threshold("Уставка контроля минимума напряжения для частоты и фазового сдвига, %", 2));
    cp1->setLayout(vlyout1);

    vlyout1 = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(cp1,"Конфигурация");
    vlyout1->addWidget(ConfTW);
    setLayout(vlyout1);
}

bool ConfDialogA1::CheckConf()
{
    return true;
}

QWidget *ConfDialogA1::Threshold(QString str, int numthr)
{
    QWidget *w = new QWidget;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QLabel *lbl = new QLabel(str);
    gb2lyout->addWidget(lbl);
    s_tqSpinBox *dspbls = WDFunc::NewSPB(this, "thr."+QString::number(numthr), 0, 100, 0.1, 1, ACONFWCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(SetThreshold(double)));
    gb2lyout->addWidget(dspbls);
    w->setLayout(gb2lyout);
    return w;
}

void ConfDialogA1::SetThreshold(double dbl)
{
    int thr;
    if ((thr = GetChNumFromObjectName(sender()->objectName()) == GENERALERROR))
    {
        DBGMSG;
        return;
    }
    switch (thr)
    {
    case 1: // % напряжения
    {
        CA1->Bci_block.duosc = dbl;
        break;
    }
    case 2: // % тока
    {
        CA1->Bci_block.diosc = dbl;
        break;
    }
    case 3: // % частоты
    {
        CA1->Bci_block.duimin = dbl;
        break;
    }
    default:
        break;
    }
}

void ConfDialogA1::SetVoltageClass(int tmpi)
{
    CA1->Bci_block.Unom = tmpi;
}

void ConfDialogA1::SetDefConf()
{
    CA1->SetDefConf();
}
