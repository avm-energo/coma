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
#include "confdialog80.h"

ConfDialog80::ConfDialog80(QWidget *parent) :
    QDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    C80 = new Config80(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);

    SetupUI();
    PrereadConf();
}

ConfDialog80::~ConfDialog80()
{
}

void ConfDialog80::FillConfData()
{
/*    int i;
    GeneralConf->Fill(); // заполнение общих полей
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

void ConfDialog80::SetupUI()
{
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QWidget *cp1 = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);

    QLabel *lbl = new QLabel ("Тип контролируемого оборудования:");
    hlyout->addWidget(lbl, 0);
    QStringList cbl = QStringList() << "1ф трансформатор/АТ" << "3ф трансформатор/АТ" << "1ф реактор" << "3ф реактор";
    s_tqComboBox *cb = WDFunc::NewCB(this, "eq_typecb", cbl, ACONFGCLR);
    cb->setMinimumWidth(70);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetEqType(int)));
    hlyout->addWidget(cb,10);
    vlyout1->addLayout(hlyout);

    QGroupBox *gb = new QGroupBox("Аналоговые");
    switch (pc.ModuleBsi.MTypeM)
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
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Осциллограммы");
    vlyout2 = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Запуск осциллограммы:");
    hlyout->addWidget(lbl);
    s_tqCheckBox *chb = WDFunc::NewChB(this, "oscchb.0", "по команде Ц", ACONFWCLR);
    connect(chb,SIGNAL(statechanged(int)),this,SLOT(SetOsc(int)));
    hlyout->addWidget(chb);
    chb = WDFunc::NewChB(this, "oscchb.1", "по дискр. входу PD1", ACONFWCLR);
    connect(chb,SIGNAL(statechanged(int)),this,SLOT(SetOsc(int)));
    hlyout->addWidget(chb);
    chb = WDFunc::NewChB(this, "oscchb.2", "по резкому изменению", ACONFWCLR);
    connect(chb,SIGNAL(statechanged(int)),this,SLOT(SetOsc(int)));
    hlyout->addWidget(chb);
    vlyout2->addLayout(hlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Прочие");
    vlyout2 = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Кол-во точек оцифровки:");
    hlyout->addWidget(lbl);
    cbl = QStringList() << "64" << "80" << "128" << "256";
    cb = WDFunc::NewCB(this, "npointscb", cbl, ACONFWCLR);
    connect(cb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetNPoints(QString)));
    hlyout->addWidget(cb);
    vlyout2->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Постоянная времени фильтрации:");
    hlyout->addWidget(lbl);
    s_tqSpinBox *spb = WDFunc::NewSPB(this, "nfiltrspb", 1, 1000, 1, 0, ACONFWCLR);
    connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetNFiltr(int)));
    hlyout->addWidget(spb);
    vlyout2->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Постоянная времени гармоник:");
    hlyout->addWidget(lbl);
    spb = WDFunc::NewSPB(this, "nhfiltrspb", 1, 100, 1, 0, ACONFWCLR);
    connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetNHFiltr(int)));
    hlyout->addWidget(spb);
    vlyout2->addLayout(hlyout);
    gb->setLayout(vlyout1);
    vlyout1->addWidget(gb);
    cp1->setLayout(vlyout1);

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(cp1,"Общие");
    lyout->addWidget(ConfTW);
    setLayout(lyout);
}

QWidget *ConfDialog80::UNom(int numunom)
{
    QWidget *w = new QWidget;
    QString NumUNomStr = QString::number(numunom);
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QLabel *lbl=new QLabel("Класс напряжения "+NumUNomStr+"-й группы, кВ:");
    gb2lyout->addWidget(lbl);
    QStringList cbl = QStringList() << "1150" << "750" << "500" << "330" << "220" << "110" << "35" << "21" << "15.75" << "11" << "10" << "6.3";
    s_tqComboBox *cb = WDFunc::NewCB(this, "unom."+NumUNomStr, cbl, ACONFWCLR);
    cb->setEditable(true);
    connect(cb,SIGNAL(textChanged(int)),this,SLOT(SetVoltageClass(int)));
    gb2lyout->addWidget(cb);
    w->setLayout(gb2lyout);
    return w;
}

// 1 - первичный ток первой группы, 2 - вторичный ток первой группы, 3,4 - то же по второй группе

QWidget *ConfDialog80::INom(int numinom)
{
    QWidget *w = new QWidget;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QString NumGroup = (numinom < 3) ? "1" : "2";
    QString Perv = (numinom%2) ? "первичные" : "вторичные";
    QString PervNum = (numinom%2) ? "1" : "2";
    QLabel *lbl=new QLabel("Номинальные "+Perv+" токи ТТ "+NumGroup+"-й группы, А: ");
    gb2lyout->addWidget(lbl, 0);
    for (int i = 0; i < 3; i++)
    {
        lbl=new QLabel(QString::number(i+10, 16).toUpper() + ":"); // A, B, C
        gb2lyout->addWidget(lbl, 1, Qt::AlignRight);
        // inom.<группа 1|2>.<тип перв/втор 1|2>.<phase>
        s_tqSpinBox *dspbls = WDFunc::NewSPB(this, "inom."+NumGroup+"."+PervNum+"."+QString::number(i), 1, 50000, 1, 0, ACONFWCLR);
        connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(SetCurrent(double)));
        gb2lyout->addWidget(dspbls, 1);
    }
    w->setLayout(gb2lyout);
    return w;
}

QWidget *ConfDialog80::Threshold(QString str, int numthr)
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

void ConfDialog80::SetOsc(int isChecked)
{
    int val;
    if ((val = GetChNumFromObjectName(sender()->objectName()) == GENERALERROR))
    {
        DBGMSG;
        return;
    }
    quint16 tmpint = 0x0001;
    tmpint = tmpint << val;
    if (isChecked == Qt::Checked)
        C80->Bci_block.ddosc |= tmpint;
    else
        C80->Bci_block.ddosc &= ~tmpint;
}

void ConfDialog80::SetThreshold(double dbl)
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
        C80->Bci_block.duosc = dbl;
        break;
    }
    case 2: // % тока
    {
        C80->Bci_block.diosc = dbl;
        break;
    }
    case 3: // % частоты
    {
        C80->Bci_block.duimin = dbl;
        break;
    }
    default:
        break;
    }
}

void ConfDialog80::SetVoltageClass(int tmpi)
{
    int vnum;
    if ((vnum = GetChNumFromObjectName(sender()->objectName()) == GENERALERROR))
    {
        DBGMSG;
        return;
    }
    if (vnum == 2) // вторая тройка напряжений
        C80->Bci_block.unom2 = tmpi;
    else
        C80->Bci_block.unom1 = tmpi;
}

void ConfDialog80::SetCurrent(double dbl)
{
    QStringList sl = sender()->objectName().split(".");
    if (sl.size() < 4) // name, group, perv, phase
    {
        DBGMSG;
        return;
    }
    bool ok;
    int group, perv, phase;
    group = sl.at(1).toInt(&ok);
    if (ok)
    {
        perv = sl.at(2).toInt(&ok);
        if (ok)
        {
            phase = sl.at(3).toInt(&ok);
            if (ok)
            {
                if (perv == 1) // первичная группа
                    C80->Bci_block.inom1[(group-1)*3+phase] = dbl;
                else
                    C80->Bci_block.inom2[(group-1)*3+phase] = dbl;
                return;
            }
        }
    }
    DBGMSG;
}

void ConfDialog80::SetEqType(int tmpi)
{
    C80->Bci_block.eq_type = tmpi;
}

void ConfDialog80::SetNPoints(QString tmpi)
{
    C80->Bci_block.npoints = tmpi.toInt();
}

void ConfDialog80::SetNFiltr(int tmpi)
{
    C80->Bci_block.nfiltr = tmpi;
}

void ConfDialog80::SetNHFiltr(int tmpi)
{
    C80->Bci_block.nhfiltr = tmpi;
}

void ConfDialog80::SetDefConf()
{
    C80->SetDefConf();
}
