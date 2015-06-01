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
#include <QMessageBox>

#include "e_confdialog.h"
#include "../config.h"
#include "../canal.h"

e_confdialog::e_confdialog(QWidget *parent) :
    QDialog(parent)
{
    Config[0] = {EBCI_MTYPE, u32_TYPE, sizeof(quint32), sizeof(Bci_block.MType)/sizeof(quint32), &(Bci_block.MType)};
    Config[1] = {EBCI_MTYPE1, u32_TYPE, sizeof(quint32), sizeof(Bci_block.MType1)/sizeof(quint32), &(Bci_block.MType1)};
    Config[2] = {EBCI_EQTYPE, u32_TYPE, sizeof(quint32), sizeof(Bci_block.eq_type)/sizeof(quint32), &(Bci_block.eq_type)};
    Config[3] = {EBCI_NPOINTS, u32_TYPE, sizeof(quint32), sizeof(Bci_block.npoints)/sizeof(quint32), &(Bci_block.npoints)};
    Config[4] = {EBCI_NFILTR, u32_TYPE, sizeof(quint32), sizeof(Bci_block.nfiltr)/sizeof(quint32), &Bci_block.nfiltr};
    Config[5] = {EBCI_NHFILTR, u32_TYPE, sizeof(quint32), sizeof(Bci_block.nhfiltr)/sizeof(quint32), &Bci_block.nhfiltr};
    Config[6] = {EBCI_DDOSC, u32_TYPE, sizeof(quint32), sizeof(Bci_block.ddosc)/sizeof(quint32), &Bci_block.ddosc};
    Config[7] = {EBCI_UNOM1, float_TYPE, sizeof(float), sizeof(Bci_block.unom1)/sizeof(float), &Bci_block.unom1};
    Config[8] = {EBCI_UNOM2, float_TYPE, sizeof(float), sizeof(Bci_block.unom2)/sizeof(float), &Bci_block.unom2};
    Config[9] = {EBCI_INOM1, float_TYPE, sizeof(float), sizeof(Bci_block.inom1)/sizeof(float), &Bci_block.inom1};
    Config[10] = {EBCI_INOM2, float_TYPE, sizeof(float), sizeof(Bci_block.inom2)/sizeof(float), &Bci_block.inom2};
    Config[11] = {EBCI_DUOSC, float_TYPE, sizeof(float), sizeof(Bci_block.duosc)/sizeof(float), &Bci_block.duosc};
    Config[12] = {EBCI_DIOSC, float_TYPE, sizeof(float), sizeof(Bci_block.diosc)/sizeof(float), &Bci_block.diosc};
    Config[13] = {EBCI_DUIMIN, float_TYPE, sizeof(float), sizeof(Bci_block.duimin)/sizeof(float), &Bci_block.duimin};
    Config[14] = {EBCI_CTYPE, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Ctype)/sizeof(quint32), &Bci_block.Ctype};
    Config[15] = {EBCI_ABS_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Abs_104)/sizeof(quint32), &Bci_block.Abs_104};
    Config[16] = {EBCI_CYCLE_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Cycle_104)/sizeof(quint32), &Bci_block.Cycle_104};
    Config[17] = {EBCI_T1_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T1_104)/sizeof(quint32), &Bci_block.T1_104};
    Config[18] = {EBCI_T2_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T2_104)/sizeof(quint32), &Bci_block.T2_104};
    Config[19] = {EBCI_T3_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T3_104)/sizeof(quint32), &Bci_block.T3_104};
    Config[20] = {EBCI_K_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.k_104)/sizeof(quint32), &Bci_block.k_104};
    Config[21] = {EBCI_W_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.w_104)/sizeof(quint32), &Bci_block.w_104};
    Config[22] = {0xFFFF, 0, 0, 0, NULL};

    Bci_defblock.MType = 4;
    Bci_defblock.MType1 = 0x000001;
    Bci_defblock.Abs_104 = 205;
    Bci_defblock.Ctype = 1;
    Bci_defblock.Cycle_104 = 5;
    Bci_defblock.T1_104 = 15;
    Bci_defblock.T2_104 = 10;
    Bci_defblock.T3_104 = 20;
    Bci_defblock.k_104 = 12;
    Bci_defblock.w_104 = 8;
    Bci_defblock.eq_type = 1;
    Bci_defblock.npoints = 128;
    Bci_defblock.nfiltr = 100;
    Bci_defblock.nhfiltr = 3;
    Bci_defblock.ddosc = 1;
    Bci_defblock.unom1 = 500;
    Bci_defblock.unom2 = 220;
    Bci_defblock.duosc = 2.0;
    Bci_defblock.diosc = 2.0;
    Bci_defblock.duimin = 0.5;
    for (int i = 0; i < 3; i++)
    {
        Bci_defblock.inom1[i] = 600;
        Bci_defblock.inom1[i+3] = 1000;
        Bci_defblock.inom2[i] = Bci_defblock.inom2[i+3] = 5;
    }

    setAttribute(Qt::WA_DeleteOnClose);
//    cn = new canal;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    lyout->addWidget(ConfTW);
    QWidget *wdgt = new QWidget;
    QGridLayout *wdgtlyout = new QGridLayout;
    QPushButton *pb1 = new QPushButton("Прочитать конфигурацию из модуля");
    connect(pb1,SIGNAL(clicked()),this,SLOT(GetBci()));
    wdgtlyout->addWidget(pb1, 0, 0, 1, 1);
    QPushButton *pb = new QPushButton("Записать конфигурацию в модуль");
    pb->setObjectName("WriteConfPB");
//    pb->setEnabled(false);
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteConfDataToModule()));
    wdgtlyout->addWidget(pb, 1, 0, 1, 1);
    pb = new QPushButton("Задать конфигурацию по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetDefConf()));
    wdgtlyout->addWidget(pb, 0, 1, 1, 1);
    pb = new QPushButton("Перейти на новую конфигурацию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetNewConf()));
    wdgtlyout->addWidget(pb, 1, 1, 1, 1);
    wdgt->setLayout(wdgtlyout);
    lyout->addWidget(wdgt);
    setLayout(lyout);
    SetupUI();
    SetDefConf();
}

void e_confdialog::GetBci()
{
    connect(cn,SIGNAL(DataReady()),this,SLOT(CheckConfAndFill()));
    cn->Send(CN_GF,NULL,0,1,Config);
}

void e_confdialog::CheckConfAndFill()
{
    disconnect(cn,SIGNAL(DataReady()),this,SLOT(CheckConfAndFill()));
    if (cn->result)
    {
        ShowErrMsg(cn->result);
        return;
    }
    FillConfData();
}

void e_confdialog::FillConfData()
{
    int i;
    QSpinBox *spb;
    s_tqspinbox *dspbls;
    s_tqComboBox *cb;
    s_tqCheckBox *chb;

    dspbls = this->findChild<s_tqspinbox *>("abs104spb");
    if (dspbls != 0)
        dspbls->setValue(Bci_block.Abs_104);
    dspbls = this->findChild<s_tqspinbox *>("cycle104spb");
    if (dspbls != 0)
        dspbls->setValue(Bci_block.Cycle_104);
    dspbls = this->findChild<s_tqspinbox *>("t1104spb");
    if (dspbls != 0)
        dspbls->setValue(Bci_block.T1_104);
    dspbls = this->findChild<s_tqspinbox *>("t2104spb");
    if (dspbls != 0)
        dspbls->setValue(Bci_block.T2_104);
    dspbls = this->findChild<s_tqspinbox *>("t3104spb");
    if (dspbls != 0)
        dspbls->setValue(Bci_block.T3_104);
    dspbls = this->findChild<s_tqspinbox *>("k104spb");
    if (dspbls != 0)
        dspbls->setValue(Bci_block.k_104);
    dspbls = this->findChild<s_tqspinbox *>("w104spb");
    if (dspbls != 0)
        dspbls->setValue(Bci_block.w_104);
    cb = this->findChild<s_tqComboBox *>("ctypecb");
    if (cb != 0)
        cb->setCurrentIndex(Bci_block.Ctype);
    cb = this->findChild<s_tqComboBox *>("eq_typecb");
    if (cb != 0)
        cb->setCurrentIndex(Bci_block.eq_type);
    chb = this->findChild<s_tqCheckBox *>("osc1chb");
    if (chb != 0)
    {
        if (Bci_block.ddosc & 0x0001)
            chb->setChecked(true);
        else
            chb->setChecked(false);
    }
    chb = this->findChild<s_tqCheckBox *>("osc2chb");
    if (chb != 0)
    {
        if (Bci_block.ddosc & 0x0002)
            chb->setChecked(true);
        else
            chb->setChecked(false);
    }
    chb = this->findChild<s_tqCheckBox *>("osc3chb");
    if (chb != 0)
    {
        if (Bci_block.ddosc & 0x0004)
            chb->setChecked(true);
        else
            chb->setChecked(false);
    }
    cb = this->findChild<s_tqComboBox *>("npointscb");
    if (cb != 0)
        cb->setCurrentText(QString::number(Bci_block.npoints));
    spb = this->findChild<QSpinBox *>("nfiltrspb");
    if (dspbls != 0)
        spb->setValue(Bci_block.nfiltr);
    spb = this->findChild<QSpinBox *>("nhfiltrspb");
    if (spb != 0)
        spb->setValue(Bci_block.nhfiltr);
    switch (pc.MType1)
    {
    case ET_0T2N: // 2 напряжения, 0 токов
    {
        cb = this->findChild<s_tqComboBox *>("unom1cb");
        if (cb != 0)
            cb->setCurrentText(QString::number(Bci_block.unom1));
        cb = this->findChild<s_tqComboBox *>("unom2cb");
        if (cb != 0)
            cb->setCurrentText(QString::number(Bci_block.unom2));
        dspbls = this->findChild<s_tqspinbox *>("duosc");
        if (dspbls != 0)
            dspbls->setValue(Bci_block.duosc);
        dspbls = this->findChild<s_tqspinbox *>("duimin");
        if (dspbls != 0)
            dspbls->setValue(Bci_block.duimin);
        break;
    }
    case ET_1T1N:
    {
        cb = this->findChild<s_tqComboBox *>("unom1cb");
        if (cb != 0)
            cb->setCurrentText(QString::number(Bci_block.unom1));
        for (i = 3; i < 6; i++)
        {
            dspbls = this->findChild<s_tqspinbox *>("inom1"+QString::number(i));
            if (dspbls != 0)
                dspbls->setValue(Bci_block.inom1[i]);
            cb = this->findChild<s_tqComboBox *>("inom2"+QString::number(i));
            QString tmps = cb->currentText();
            if (cb != 0)
                cb->setCurrentText(QString::number(Bci_block.inom2[i]));
            tmps = cb->currentText();
        }
        dspbls = this->findChild<s_tqspinbox *>("duosc");
        if (dspbls != 0)
            dspbls->setValue(Bci_block.duosc);
        dspbls = this->findChild<s_tqspinbox *>("diosc");
        if (dspbls != 0)
            dspbls->setValue(Bci_block.diosc);
        dspbls = this->findChild<s_tqspinbox *>("duimin");
        if (dspbls != 0)
            dspbls->setValue(Bci_block.duimin);
        break;
    }
    case ET_2T0N:
    {
        for (i = 0; i < 6; i++)
        {
            dspbls = this->findChild<s_tqspinbox *>("inom1"+QString::number(i));
            if (dspbls != 0)
                dspbls->setValue(Bci_block.inom1[i]);
            cb = this->findChild<s_tqComboBox *>("inom2"+QString::number(i));
            if (cb != 0)
                cb->setCurrentText(QString::number(Bci_block.inom2[i]));
        }
        dspbls = this->findChild<s_tqspinbox *>("duosc");
        if (dspbls != 0)
            dspbls->setValue(Bci_block.duosc);
        dspbls = this->findChild<s_tqspinbox *>("diosc");
        if (dspbls != 0)
            dspbls->setValue(Bci_block.diosc);
        dspbls = this->findChild<s_tqspinbox *>("duimin");
        if (dspbls != 0)
            dspbls->setValue(Bci_block.duimin);
        break;
    }
    default:
        break;
    }
}

void e_confdialog::SetupUI()
{
    int i;
    QGridLayout *lyout1 = new QGridLayout;
//    QGridLayout *lyout2 = new QGridLayout;
//    QGridLayout *lyout3 = new QGridLayout;
    QGridLayout *lyout4 = new QGridLayout;
    QWidget *cp1 = new QWidget;
//    QWidget *cp2 = new QWidget;
//    QWidget *cp3 = new QWidget;
    QWidget *cp4 = new QWidget;
    QTabWidget *ConfTW = this->findChild<QTabWidget *>("conftw");
    if (ConfTW == 0)
        return;
    ConfTW->addTab(cp1,"Общие");
    ConfTW->addTab(cp4,"104");
    QGroupBox *gb = new QGroupBox;
    QVBoxLayout *gblyout = new QVBoxLayout;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QGridLayout *gb3lyout = new QGridLayout;
    QLabel *lbl = new QLabel;
    s_tqComboBox *cb = new s_tqComboBox;
    QStringList cbl;
    QStringListModel *cblm = new QStringListModel;
    QSpinBox *spb = new QSpinBox;
    s_tqspinbox *dspbls;
    s_tqCheckBox *chb;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    gb3lyout = new QGridLayout;
    gb3lyout->setColumnStretch(2, 50);
    lbl = new QLabel("Адрес базовой станции:");
    gb3lyout->addWidget(lbl,0,0,1,1,Qt::AlignRight);
    dspbls = new s_tqspinbox;
    dspbls->setObjectName("abs104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(65535);
    dspbls->setAData(0);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(Set104(double,s_tqspinbox*)));
    gb3lyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);
    lbl = new QLabel("Интервал циклического опроса:");
    gb3lyout->addWidget(lbl,1,0,1,1,Qt::AlignRight);
    dspbls = new s_tqspinbox;
    dspbls->setObjectName("cycle104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setAData(1);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(Set104(double,s_tqspinbox*)));
    gb3lyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,1,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t1:");
    gb3lyout->addWidget(lbl,2,0,1,1,Qt::AlignRight);
    dspbls = new s_tqspinbox;
    dspbls->setObjectName("t1104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setAData(2);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(Set104(double,s_tqspinbox*)));
    gb3lyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,2,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t2:");
    gb3lyout->addWidget(lbl,3,0,1,1,Qt::AlignRight);
    dspbls = new s_tqspinbox;
    dspbls->setObjectName("t2104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setAData(3);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(Set104(double,s_tqspinbox*)));
    gb3lyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,3,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t3:");
    gb3lyout->addWidget(lbl,4,0,1,1,Qt::AlignRight);
    dspbls = new s_tqspinbox;
    dspbls->setObjectName("t3104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setAData(4);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(Set104(double,s_tqspinbox*)));
    gb3lyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,4,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    gb3lyout->addWidget(lbl,5,0,1,1,Qt::AlignRight);
    dspbls = new s_tqspinbox;
    dspbls->setObjectName("k104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setAData(5);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(Set104(double,s_tqspinbox*)));
    gb3lyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,5,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    gb3lyout->addWidget(lbl,6,0,1,1,Qt::AlignRight);
    dspbls = new s_tqspinbox;
    dspbls->setObjectName("w104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setAData(6);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(Set104(double,s_tqspinbox*)));
    gb3lyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,6,2,1,1);
    lbl = new QLabel("Тип синхр. времени:");
    gb3lyout->addWidget(lbl,7,0,1,1,Qt::AlignRight);
    cb->setObjectName("ctypecb");
    cbl = QStringList() << "SNTPIP2+PPS" << "SNTPIP1+PPS" << "SNTPIP1";
    cblm = new QStringListModel;
    cblm->setStringList(cbl);
    cb->setModel(cblm);
    cb->setMinimumWidth(70);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    gb3lyout->addWidget(cb, 7, 1, 1, 2);
    gb->setLayout(gb3lyout);
    lyout4->addWidget(gb, 0, 0, 1, 1);

    lbl = new QLabel ("Тип контролируемого оборудования:");
    lyout1->addWidget(lbl,0,0,1,1);
    cbl = QStringList() << "1ф трансформатор/АТ" << "3ф трансформатор/АТ" << "1ф реактор" << "3ф реактор";
    cblm = new QStringListModel;
    cblm->setStringList(cbl);
    cb = new s_tqComboBox;
    cb->setObjectName("eq_typecb");
    cb->setModel(cblm);
    cb->setMinimumWidth(70);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetEqType(int)));
    lyout1->addWidget(cb,0,1,1,1);

    gb = new QGroupBox("Аналоговые");
    gblyout = new QVBoxLayout;
    switch (pc.MType1)
    {
    case ET_0T2N:
    {
        gb2lyout = new QHBoxLayout;
        lbl=new QLabel("Класс напряжения 1-й группы, кВ:");
        gb2lyout->addWidget(lbl);
        cbl = QStringList() << "1150" << "750" << "500" << "330" << "220" << "110" << "35" << "21" << "15.75" << "11" << "10" << "6.3";
        cblm = new QStringListModel;
        cblm->setStringList(cbl);
        cb = new s_tqComboBox;
        cb->setObjectName("unom1cb");
        cb->setModel(cblm);
        cb->setEditable(true);
        cb->setAData(0);
        connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetVoltageClass(int,s_tqComboBox*)));
        gb2lyout->addWidget(cb);
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl=new QLabel("Класс напряжения 2-й группы, кВ:");
        gb2lyout->addWidget(lbl);
        cb = new s_tqComboBox;
        cb->setObjectName("unom2cb");
        cb->setModel(cblm);
        cb->setEditable(true);
        cb->setAData(1);
        connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetVoltageClass(int,s_tqComboBox*)));
        gb2lyout->addWidget(cb);
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl = new QLabel("Уставка скачка напряжения для запуска осциллографирования, %");
        gb2lyout->addWidget(lbl);
        dspbls = new s_tqspinbox;
        dspbls->setObjectName("duosc");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(0);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetThreshold(double,s_tqspinbox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl = new QLabel("Уставка порога мин. уровня для определения частоты, %");
        gb2lyout->addWidget(lbl);
        dspbls = new s_tqspinbox;
        dspbls->setObjectName("duimin");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(2);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetThreshold(double,s_tqspinbox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);
        break;
    }
    case ET_1T1N:
    {
        gb2lyout = new QHBoxLayout;
        lbl=new QLabel("Класс напряжения 1-й группы, кВ:");
        gb2lyout->addWidget(lbl);
        cbl = QStringList() << "1150" << "750" << "500" << "330" << "220" << "110" << "35" << "21" << "15.75" << "11" << "10" << "6.3";
        cblm = new QStringListModel;
        cblm->setStringList(cbl);
        cb = new s_tqComboBox;
        cb->setObjectName("unom1cb");
        cb->setModel(cblm);
        cb->setEditable(true);
        cb->setAData(0);
        connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetVoltageClass(int,s_tqComboBox*)));
        gb2lyout->addWidget(cb);
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные первичные токи ТТ 2-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        for (i = 3; i < 6; i++)
        {
            lbl=new QLabel(QString::number(i+7, 16).toUpper() + ":"); // A, B, C
            gb2lyout->addWidget(lbl, Qt::AlignRight);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("inom1"+QString::number(i));
            dspbls->setSingleStep(1);
            dspbls->setMinimum(1);
            dspbls->setMaximum(50000);
            dspbls->setDecimals(0);
            dspbls->setAData(i);
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetCurrent(double,s_tqspinbox*)));
            gb2lyout->addWidget(dspbls);
        }
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные вторичные токи ТТ 2-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        cbl = QStringList() << "1" << "5";
        cblm = new QStringListModel;
        cblm->setStringList(cbl);
        for (i = 3; i < 6; i++)
        {
            lbl=new QLabel(QString::number(i+7, 16).toUpper() + ":"); // A, B, C
            gb2lyout->setAlignment(lbl, Qt::AlignRight);
            gb2lyout->addWidget(lbl);
            cb = new s_tqComboBox;
            cb->setObjectName("inom2"+QString::number(i));
            cb->setModel(cblm);
            cb->setAData(i);
            connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetSecCurrent(int,s_tqComboBox*)));
            gb2lyout->addWidget(cb);
        }
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl = new QLabel("Уставка скачка напряжения для запуска осциллографирования, %");
        gb2lyout->addWidget(lbl);
        dspbls = new s_tqspinbox;
        dspbls->setObjectName("duosc");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(0);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetThreshold(double,s_tqspinbox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl = new QLabel("Уставка скачка тока для запуска осциллографирования, %");
        gb2lyout->addWidget(lbl);
        dspbls = new s_tqspinbox;
        dspbls->setObjectName("diosc");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(1);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetThreshold(double,s_tqspinbox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl = new QLabel("Уставка порога мин. уровня для определения частоты, %");
        gb2lyout->addWidget(lbl);
        dspbls = new s_tqspinbox;
        dspbls->setObjectName("duimin");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(2);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetThreshold(double,s_tqspinbox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);
        break;
    }
    case ET_2T0N:
    {
        lbl=new QLabel("Номинальные первичные токи ТТ 1-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        for (i = 0; i < 3; i++)
        {
            lbl=new QLabel(QString::number(i+10, 16).toUpper() + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("inom1"+QString::number(i));
            dspbls->setSingleStep(1);
            dspbls->setMinimum(1);
            dspbls->setMaximum(50000);
            dspbls->setDecimals(0);
            dspbls->setAData(i);
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetCurrent(double,s_tqspinbox*)));
            gb2lyout->addWidget(dspbls);
        }
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные вторичные токи ТТ 1-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        cbl = QStringList() << "1" << "5";
        cblm = new QStringListModel;
        cblm->setStringList(cbl);
        for (i = 0; i < 3; i++)
        {
            lbl=new QLabel(QString::number(i+10, 16).toUpper() + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            cb = new s_tqComboBox;
            cb->setObjectName("inom2"+QString::number(i));
            cb->setModel(cblm);
            cb->setAData(i);
            connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetSecCurrent(int,s_tqComboBox*)));
            gb2lyout->addWidget(cb);
        }
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные первичные токи ТТ 2-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        for (i = 3; i < 6; i++)
        {
            lbl=new QLabel(QString::number(i+7, 16).toUpper() + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("inom1"+QString::number(i));
            dspbls->setSingleStep(1);
            dspbls->setMinimum(1);
            dspbls->setMaximum(50000);
            dspbls->setDecimals(0);
            dspbls->setAData(i);
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetCurrent(double,s_tqspinbox*)));
            gb2lyout->addWidget(dspbls);
        }
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные вторичные токи ТТ 2-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        cbl = QStringList() << "1" << "5";
        cblm = new QStringListModel;
        cblm->setStringList(cbl);
        for (i = 3; i < 6; i++)
        {
            lbl=new QLabel(QString::number(i+7, 16).toUpper() + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            cb = new s_tqComboBox;
            cb->setObjectName("inom2"+QString::number(i));
            cb->setModel(cblm);
            cb->setAData(i);
            connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetSecCurrent(int,s_tqComboBox*)));
            gb2lyout->addWidget(cb);
        }
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl = new QLabel("Уставка скачка тока для запуска осциллографирования, %");
        gb2lyout->addWidget(lbl);
        dspbls = new s_tqspinbox;
        dspbls->setObjectName("diosc");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(1);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetThreshold(double,s_tqspinbox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl = new QLabel("Уставка порога мин. уровня для определения частоты, %");
        gb2lyout->addWidget(lbl);
        dspbls = new s_tqspinbox;
        dspbls->setObjectName("duimin");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(2);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetThreshold(double,s_tqspinbox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);
        break;
    }
    default:
        break;
    }
    gb->setLayout(gblyout);
    lyout1->addWidget(gb,1,0,1,2);

    gb = new QGroupBox("Осциллограммы");
    gblyout = new QVBoxLayout;
    gb2lyout = new QHBoxLayout;
    lbl = new QLabel("Запуск осциллограммы:");
    gb2lyout->addWidget(lbl);
    chb = new s_tqCheckBox;
    chb->setObjectName("osc1chb");
    chb->setText("по команде Ц");
    chb->setAData(0);
    connect(chb,SIGNAL(statechanged(int,s_tqCheckBox*)),this,SLOT(SetOsc(int,s_tqCheckBox*)));
    gb2lyout->addWidget(chb);
    chb = new s_tqCheckBox;
    chb->setObjectName("osc2chb");
    chb->setText("по дискр. входу PD1");
    chb->setAData(1);
    connect(chb,SIGNAL(statechanged(int,s_tqCheckBox*)),this,SLOT(SetOsc(int,s_tqCheckBox*)));
    gb2lyout->addWidget(chb);
    chb = new s_tqCheckBox;
    chb->setObjectName("osc3chb");
    chb->setText("по резкому изменению");
    chb->setAData(2);
    connect(chb,SIGNAL(statechanged(int,s_tqCheckBox*)),this,SLOT(SetOsc(int,s_tqCheckBox*)));
    gb2lyout->addWidget(chb);
    gblyout->addLayout(gb2lyout);
    gb->setLayout(gblyout);
    lyout1->addWidget(gb,2,0,1,2);

    gb = new QGroupBox("Прочие");
    gb3lyout = new QGridLayout;
    lbl = new QLabel("Кол-во точек оцифровки:");
    gb3lyout->addWidget(lbl, 0, 0, 1, 1);
    cbl = QStringList() << "64" << "80" << "128" << "256";
    cblm = new QStringListModel;
    cblm->setStringList(cbl);
    cb = new s_tqComboBox;
    cb->setObjectName("npointscb");
    cb->setModel(cblm);
    connect(cb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetNPoints(QString)));
    gb3lyout->addWidget(cb,0,1,1,1);
    lbl = new QLabel("Постоянная времени фильтрации:");
    gb3lyout->addWidget(lbl, 1, 0, 1, 1);
    spb = new QSpinBox;
    spb->setObjectName("nfiltrspb");
    spb->setMinimum(1);
    spb->setMaximum(1000);
    connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetNFiltr(int)));
    gb3lyout->addWidget(spb,1,1,1,1);
    lbl = new QLabel("Постоянная времени гармоник:");
    gb3lyout->addWidget(lbl, 2, 0, 1, 1);
    spb = new QSpinBox;
    spb->setObjectName("nhfiltrspb");
    spb->setMinimum(1);
    spb->setMaximum(10);
    connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetNHFiltr(int)));
    gb3lyout->addWidget(spb,2,1,1,1);
    gb->setLayout(gb3lyout);
    lyout1->addWidget(gb,3,0,1,2);

    cp1->setLayout(lyout1);
//    cp2->setLayout(lyout2);
//    cp3->setLayout(lyout3);
    cp4->setLayout(lyout4);
}

void e_confdialog::SetOsc(int isChecked, s_tqCheckBox *ptr)
{
    quint16 tmpint = 0x0001;
    tmpint = tmpint << ptr->getAData().toInt();
    if (isChecked == Qt::Checked)
        Bci_block.ddosc |= tmpint;
    else
        Bci_block.ddosc &= ~tmpint;
}

void e_confdialog::SetThreshold(double dbl, s_tqspinbox *ptr)
{
    switch (ptr->getAData().toInt())
    {
    case 0: // % напряжения
    {
        Bci_block.duosc = dbl;
        break;
    }
    case 1:
    {
        Bci_block.diosc = dbl;
        break;
    }
    case 2:
    {
        Bci_block.duimin = dbl;
        break;
    }
    default:
        break;
    }
}

void e_confdialog::SetVoltageClass(int tmpi, s_tqComboBox *ptr)
{
    Q_UNUSED(tmpi);
    if (ptr->getAData().toInt())
        Bci_block.unom2 = ptr->currentText().toInt();
    else
        Bci_block.unom1 = ptr->currentText().toInt();
}

void e_confdialog::SetCurrent(double dbl, s_tqspinbox *ptr)
{
    Bci_block.inom1[ptr->getAData().toInt()] = dbl;
}

void e_confdialog::SetSecCurrent(int tmpi, s_tqComboBox *ptr)
{
    Q_UNUSED(tmpi);
    Bci_block.inom2[ptr->getAData().toInt()] = ptr->currentText().toInt();
}

void e_confdialog::SetEqType(int tmpi)
{
    Bci_block.eq_type = tmpi;
}

void e_confdialog::SetNPoints(QString tmpi)
{
    Bci_block.npoints = tmpi.toInt();
}

void e_confdialog::SetNFiltr(int tmpi)
{
    Bci_block.nfiltr = tmpi;
}

void e_confdialog::SetNHFiltr(int tmpi)
{
    Bci_block.nhfiltr = tmpi;
}

void e_confdialog::Set104(double dbl, s_tqspinbox *ptr)
{
    switch (ptr->getAData().toInt())
    {
    case 0:
    {
        Bci_block.Abs_104=dbl;
        break;
    }
    case 1:
    {
        Bci_block.Cycle_104=dbl;
        break;
    }
    case 2:
    {
        Bci_block.T1_104=dbl;
        break;
    }
    case 3:
    {
        Bci_block.T2_104=dbl;
        break;
    }
    case 4:
    {
        Bci_block.T3_104=dbl;
        break;
    }
    case 5:
    {
        Bci_block.k_104=dbl;
        break;
    }
    case 6:
    {
        Bci_block.w_104=dbl;
        break;
    }
    default:
        break;
    }
}

void e_confdialog::SetCType(int num)
{
    Bci_block.Ctype = num;
}

void e_confdialog::WriteConfDataToModule()
{
    connect(cn,SIGNAL(DataReady()),this,SLOT(WriteCompleted()));
    cn->Send(CN_WF, &Bci_block, sizeof(Bci_block), 2, Config);
}

void e_confdialog::WriteCompleted()
{
    disconnect(cn,SIGNAL(DataReady()),this,SLOT(WriteCompleted()));
    if (!cn->result)
        QMessageBox::information(this,"Успешно!","Операция проведена успешно!");
    else
        ShowErrMsg(cn->result);
}

void e_confdialog::SetNewConf()
{
    connect(cn,SIGNAL(DataReady()),this,SLOT(UpdateBsi()));
    cn->Send(CN_Cnc);
}

void e_confdialog::UpdateBsi()
{
    disconnect(cn,SIGNAL(DataReady()),this,SLOT(UpdateBsi()));
    emit BsiIsNeedToBeAcquiredAndChecked();
}

void e_confdialog::ShowErrMsg(int ermsg)
{
    QMessageBox::critical(this,"error!",pc.errmsgs.at(ermsg));
}

void e_confdialog::SetDefConf()
{
    memcpy(&Bci_block, &Bci_defblock, sizeof(Bci));
    FillConfData();
}
