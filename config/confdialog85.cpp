#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGridLayout>
#include <QVBoxLayout>
#include "../widgets/messagebox.h"
#include "../widgets/s_tqcombobox.h"
#include "../widgets/wd_func.h"
#include "../widgets/mystackedwidget.h"
#include "confdialog85.h"

ConfDialog85::ConfDialog85(QVector<publicclass::DataRec> &S2Config, QWidget *parent) :
    AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    this->S2Config = &S2Config;
    C85 = new Config85(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);

    SetupUI();
    PrereadConf();
}

ConfDialog85::~ConfDialog85()
{
}

void ConfDialog85::Fill()
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

void ConfDialog85::FillBack()
{

}

void ConfDialog85::SetupUI()
{
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QWidget *cp4 = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFGCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);
    cp4->setStyleSheet(tmps);

    glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутируемого оборудования:"), 0,0,1,1);
    QStringList cbl = QStringList() << "0. Свободное конфигурирование" << "1. Конденсаторная батарея (фильтр)" \
                                    << "2. Реактор шунтирующий" << "3. Силовой трансформатор" \
                                    << "4. Линия электропередачи";
    s_tqComboBox *cb = WDFunc::NewCB(this, "eq_typecb", cbl, ACONFGCLR);
    cb->setMinimumWidth(70);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetEqType(int)));
    glyout->addWidget(cb,0,1,1,1);

    cbl = QStringList() << "Несинхронное" << "Синхронное";
    glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутаций:"), 1,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "включение:"), 0);
    hlyout->addWidget(WDFunc::NewCB(this, "cbvkl", cbl), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "отключение:"), 0);
    hlyout->addWidget(WDFunc::NewCB(this, "cbotkl", cbl), 1);
    glyout->addLayout(hlyout,1,1,1,1);
    hlyout = new QHBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Наличие напряжения на стороне нагрузки"),2,0,1,1);
    QButtonGroup *bg = new QButtonGroup;
    QRadioButton *rb = new QRadioButton("Есть");
    rb->setObjectName("u2yes");
    bg->addButton(rb);
    hlyout->addWidget(rb);
    rb = new QRadioButton("Нет");
    rb->setObjectName("u2no");
    bg->addButton(rb);
    hlyout->addWidget(rb);
    glyout->addLayout(hlyout, 2, 1, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(this, "Класс напряжения сети, кВ:"), 3,0,1,1);
    cbl = QStringList() << "750" << "550" << "330" << "220" << "110" << "35" << "10";
    glyout->addWidget(WDFunc::NewCB(this, "unomcb", cbl, ACONFGCLR),3,1,1,1);

    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный ток, А:"),4,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "первичный:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "inom1", 10, 50000, 10, 0, ACONFGCLR), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "вторичный:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "inom2", 10, 50000, 10, 0, ACONFGCLR), 1);
    glyout->addLayout(hlyout,4,1,1,1);

    vlyout1->addLayout(glyout);
    MyStackedWidget *stw = new MyStackedWidget;
    stw->setObjectName("mainstw");
    vlyout1->addWidget(stw, 1);
    vlyout1->addStretch(1);
    cp1->setLayout(vlyout1);

    vlyout1 = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный ток, А:"),0,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "inomspb", 10, 50000, 10, 0),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Максимальный коммутируемый ток, кА:"),1,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "ikzspb", 1, 150, 0.1, 1),1,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Минимальное время горения дуги, мс:"),2,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "tarcspb", 10, 50000, 10, 0),2,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Собственное время отключения (А,В,С), мс:"),3,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tsoff1", 1, 500, 0.1, 1, ACONFGCLR), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tsoff2", 1, 500, 0.1, 1, ACONFGCLR), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tsoff3", 1, 500, 0.1, 1, ACONFGCLR), 1);
    glyout->addLayout(hlyout, 3, 1, 1, 1);



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
    QLabel *lbl = new QLabel("Запуск осциллограммы:");
    hlyout->addWidget(lbl);
    QCheckBox *chb = WDFunc::NewChB(this, "oscchb.0", "по команде Ц", ACONFWCLR);
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
    QDoubleSpinBox *spb = WDFunc::NewSPB(this, "nfiltrspb", 1, 1000, 1, 0, ACONFWCLR);
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
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    cp1->setLayout(vlyout1);

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(cp1,"Основные");
    ConfTW->addTab(cp2,"Параметры выключателя");
    ConfTW->addTab(cp1,"Доп. параметры");
    ConfTW->addTab(cp1,"Уставки сигнализации");
    lyout->addWidget(ConfTW);
    setLayout(lyout);
}

void ConfDialog85::CheckConf()
{
}

QWidget *ConfDialog85::UNom(int numunom)
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

QWidget *ConfDialog85::INom(int numinom)
{
    QWidget *w = new QWidget;
/*    QHBoxLayout *gb2lyout = new QHBoxLayout;
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
        QDoubleSpinBox *dspbls = WDFunc::NewSPB(this, "inom."+NumGroup+"."+PervNum+"."+QString::number(i), 1, 50000, 1, 0, ACONFWCLR);
        connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(SetCurrent(double)));
        gb2lyout->addWidget(dspbls, 1);
    }
    w->setLayout(gb2lyout);*/
    return w;
}

QWidget *ConfDialog85::Threshold(QString str, int numthr)
{
    QWidget *w = new QWidget;
/*    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QLabel *lbl = new QLabel(str);
    gb2lyout->addWidget(lbl);
    QDoubleSpinBox *dspbls = WDFunc::NewSPB(this, "thr."+QString::number(numthr), 0, 100, 0.1, 1, ACONFWCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(SetThreshold(double)));
    gb2lyout->addWidget(dspbls);
    w->setLayout(gb2lyout);*/
    return w;
}

void ConfDialog85::SetOsc(int isChecked)
{
/*    int val;
    if ((val = GetChNumFromObjectName(sender()->objectName()) == GENERALERROR))
    {
        DBGMSG;
        return;
    }
    quint16 tmpint = 0x0001;
    tmpint = tmpint << val;
    if (isChecked == Qt::Checked)
        C85->Bci_block.ddosc |= tmpint;
    else
        C85->Bci_block.ddosc &= ~tmpint;*/
}

void ConfDialog85::SetThreshold(double dbl)
{
/*    int thr;
    if ((thr = GetChNumFromObjectName(sender()->objectName()) == GENERALERROR))
    {
        DBGMSG;
        return;
    }
    switch (thr)
    {
    case 1: // % напряжения
    {
        C85->Bci_block.duosc = dbl;
        break;
    }
    case 2: // % тока
    {
        C85->Bci_block.diosc = dbl;
        break;
    }
    case 3: // % частоты
    {
        C85->Bci_block.duimin = dbl;
        break;
    }
    default:
        break;
    }*/
}

void ConfDialog85::SetVoltageClass(int tmpi)
{
/*    int vnum;
    if ((vnum = GetChNumFromObjectName(sender()->objectName()) == GENERALERROR))
    {
        DBGMSG;
        return;
    }
    if (vnum == 2) // вторая тройка напряжений
        C85->Bci_block.unom2 = tmpi;
    else
        C85->Bci_block.unom1 = tmpi;*/
}

void ConfDialog85::SetCurrent(double dbl)
{
/*   QStringList sl = sender()->objectName().split(".");
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
                    C85->Bci_block.inom1[(group-1)*3+phase] = dbl;
                else
                    C85->Bci_block.inom2[(group-1)*3+phase] = dbl;
                return;
            }
        }
    }
    DBGMSG;*/
}

void ConfDialog85::SetEqType(int tmpi)
{
//    C85->Bci_block.eq_type = tmpi;
}

void ConfDialog85::SetNPoints(QString tmpi)
{
//    C85->Bci_block.npoints = tmpi.toInt();
}

void ConfDialog85::SetNFiltr(int tmpi)
{
//    C85->Bci_block.nfiltr = tmpi;
}

void ConfDialog85::SetNHFiltr(int tmpi)
{
//    C85->Bci_block.nhfiltr = tmpi;
}

void ConfDialog85::SetDefConf()
{
//    C85->SetDefConf();
}
