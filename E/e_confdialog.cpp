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
#include <QCoreApplication>
#include <QFileDialog>
#include "e_confdialog.h"
#include "../canal.h"

e_confdialog::e_confdialog(QWidget *parent) :
    QDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    econf = new e_config;
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    lyout->addWidget(ConfTW);
    QWidget *wdgt = new QWidget;
    QGridLayout *wdgtlyout = new QGridLayout;
    QPushButton *pb1 = new QPushButton("Прочитать из модуля");
    connect(pb1,SIGNAL(clicked()),this,SLOT(GetBci()));
    if (pc.Emul)
        pb1->setEnabled(false);
    wdgtlyout->addWidget(pb1, 0, 0, 1, 1);
    QPushButton *pb = new QPushButton("Записать в модуль");
    pb->setObjectName("WriteConfPB");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteConfDataToModule()));
    if (pc.Emul)
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 1, 1, 1);
    pb = new QPushButton("Прочитать из файла");
    pb->setIcon(QIcon(":/load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadConf()));
    wdgtlyout->addWidget(pb, 0, 2, 1, 1);
    pb = new QPushButton("Записать в файл");
    pb->setIcon(QIcon(":/save.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveConf()));
    wdgtlyout->addWidget(pb, 0, 3, 1, 1);
    pb = new QPushButton("Задать конфигурацию по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetDefConf()));
    wdgtlyout->addWidget(pb, 1, 0, 1, 2);
    pb = new QPushButton("Перейти на новую конфигурацию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetNewConf()));
    if (pc.Emul)
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 1, 2, 1, 2);
    wdgt->setLayout(wdgtlyout);
    lyout->addWidget(wdgt);
    setLayout(lyout);
    SetupUI();
    if ((pc.ModuleBsi.Hth & HTH_CONFIG) || (pc.Emul)) // если установлен признак отсутствия конфигурации
        SetDefConf();
    else
        GetBci();
}

void e_confdialog::GetBci()
{
    cn->Send(CN_GF,NULL,0,1,econf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        FillConfData();
}

void e_confdialog::FillConfData()
{
    int i;
    QSpinBox *spb;
    s_tqSpinBox *dspbls;
    s_tqComboBox *cb;
    s_tqCheckBox *chb;

    dspbls = this->findChild<s_tqSpinBox *>("abs104spb");
    if (dspbls != 0)
        dspbls->setValue(econf->Bci_block.Abs_104);
    dspbls = this->findChild<s_tqSpinBox *>("cycle104spb");
    if (dspbls != 0)
        dspbls->setValue(econf->Bci_block.Cycle_104);
    dspbls = this->findChild<s_tqSpinBox *>("t1104spb");
    if (dspbls != 0)
        dspbls->setValue(econf->Bci_block.T1_104);
    dspbls = this->findChild<s_tqSpinBox *>("t2104spb");
    if (dspbls != 0)
        dspbls->setValue(econf->Bci_block.T2_104);
    dspbls = this->findChild<s_tqSpinBox *>("t3104spb");
    if (dspbls != 0)
        dspbls->setValue(econf->Bci_block.T3_104);
    dspbls = this->findChild<s_tqSpinBox *>("k104spb");
    if (dspbls != 0)
        dspbls->setValue(econf->Bci_block.k_104);
    dspbls = this->findChild<s_tqSpinBox *>("w104spb");
    if (dspbls != 0)
        dspbls->setValue(econf->Bci_block.w_104);
    cb = this->findChild<s_tqComboBox *>("ctypecb");
    if (cb != 0)
        cb->setCurrentIndex(econf->Bci_block.Ctype);
    cb = this->findChild<s_tqComboBox *>("eq_typecb");
    if (cb != 0)
        cb->setCurrentIndex(econf->Bci_block.eq_type);
    chb = this->findChild<s_tqCheckBox *>("osc1chb");
    if (chb != 0)
    {
        if (econf->Bci_block.ddosc & 0x0001)
            chb->setChecked(true);
        else
            chb->setChecked(false);
    }
    chb = this->findChild<s_tqCheckBox *>("osc2chb");
    if (chb != 0)
    {
        if (econf->Bci_block.ddosc & 0x0002)
            chb->setChecked(true);
        else
            chb->setChecked(false);
    }
    chb = this->findChild<s_tqCheckBox *>("osc3chb");
    if (chb != 0)
    {
        if (econf->Bci_block.ddosc & 0x0004)
            chb->setChecked(true);
        else
            chb->setChecked(false);
    }
    cb = this->findChild<s_tqComboBox *>("npointscb");
    if (cb != 0)
        cb->setCurrentText(QString::number(econf->Bci_block.npoints));
    spb = this->findChild<QSpinBox *>("nfiltrspb");
    if (dspbls != 0)
        spb->setValue(econf->Bci_block.nfiltr);
    spb = this->findChild<QSpinBox *>("nhfiltrspb");
    if (spb != 0)
        spb->setValue(econf->Bci_block.nhfiltr);
    switch (pc.ModuleBsi.MType1)
    {
    case ET_0T2N: // 2 напряжения, 0 токов
    {
        cb = this->findChild<s_tqComboBox *>("unom1cb");
        if (cb != 0)
            cb->setCurrentText(QString::number(econf->Bci_block.unom1));
        cb = this->findChild<s_tqComboBox *>("unom2cb");
        if (cb != 0)
            cb->setCurrentText(QString::number(econf->Bci_block.unom2));
        dspbls = this->findChild<s_tqSpinBox *>("duosc");
        if (dspbls != 0)
            dspbls->setValue(econf->Bci_block.duosc);
        dspbls = this->findChild<s_tqSpinBox *>("duimin");
        if (dspbls != 0)
            dspbls->setValue(econf->Bci_block.duimin);
        break;
    }
    case ET_1T1N:
    {
        cb = this->findChild<s_tqComboBox *>("unom1cb");
        if (cb != 0)
            cb->setCurrentText(QString::number(econf->Bci_block.unom1));
        for (i = 3; i < 6; i++)
        {
            dspbls = this->findChild<s_tqSpinBox *>("inom1"+QString::number(i));
            if (dspbls != 0)
                dspbls->setValue(econf->Bci_block.inom1[i]);
            cb = this->findChild<s_tqComboBox *>("inom2"+QString::number(i));
            QString tmps = cb->currentText();
            if (cb != 0)
                cb->setCurrentText(QString::number(econf->Bci_block.inom2[i]));
            tmps = cb->currentText();
        }
        dspbls = this->findChild<s_tqSpinBox *>("duosc");
        if (dspbls != 0)
            dspbls->setValue(econf->Bci_block.duosc);
        dspbls = this->findChild<s_tqSpinBox *>("diosc");
        if (dspbls != 0)
            dspbls->setValue(econf->Bci_block.diosc);
        dspbls = this->findChild<s_tqSpinBox *>("duimin");
        if (dspbls != 0)
            dspbls->setValue(econf->Bci_block.duimin);
        break;
    }
    case ET_2T0N:
    {
        for (i = 0; i < 6; i++)
        {
            dspbls = this->findChild<s_tqSpinBox *>("inom1"+QString::number(i));
            if (dspbls != 0)
                dspbls->setValue(econf->Bci_block.inom1[i]);
            cb = this->findChild<s_tqComboBox *>("inom2"+QString::number(i));
            if (cb != 0)
                cb->setCurrentText(QString::number(econf->Bci_block.inom2[i]));
        }
        dspbls = this->findChild<s_tqSpinBox *>("duosc");
        if (dspbls != 0)
            dspbls->setValue(econf->Bci_block.duosc);
        dspbls = this->findChild<s_tqSpinBox *>("diosc");
        if (dspbls != 0)
            dspbls->setValue(econf->Bci_block.diosc);
        dspbls = this->findChild<s_tqSpinBox *>("duimin");
        if (dspbls != 0)
            dspbls->setValue(econf->Bci_block.duimin);
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
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
//    cp2->setStyleSheet(tmps);
//    cp3->setStyleSheet(tmps);
    cp4->setStyleSheet(tmps);
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
    s_tqSpinBox *dspbls;
    s_tqCheckBox *chb;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    gb3lyout = new QGridLayout;
    gb3lyout->setColumnStretch(2, 50);
    lbl = new QLabel("Адрес базовой станции:");
    gb3lyout->addWidget(lbl,0,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("abs104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(65535);
    dspbls->setAData(0);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(Set104(double,s_tqSpinBox*)));
    gb3lyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);
    lbl = new QLabel("Интервал циклического опроса:");
    gb3lyout->addWidget(lbl,1,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("cycle104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setAData(1);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(Set104(double,s_tqSpinBox*)));
    gb3lyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,1,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t1:");
    gb3lyout->addWidget(lbl,2,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("t1104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setAData(2);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(Set104(double,s_tqSpinBox*)));
    gb3lyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,2,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t2:");
    gb3lyout->addWidget(lbl,3,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("t2104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setAData(3);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(Set104(double,s_tqSpinBox*)));
    gb3lyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,3,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t3:");
    gb3lyout->addWidget(lbl,4,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("t3104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setAData(4);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(Set104(double,s_tqSpinBox*)));
    gb3lyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,4,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    gb3lyout->addWidget(lbl,5,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("k104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setAData(5);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(Set104(double,s_tqSpinBox*)));
    gb3lyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,5,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    gb3lyout->addWidget(lbl,6,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("w104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setAData(6);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(Set104(double,s_tqSpinBox*)));
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
    tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
    cb->setStyleSheet(tmps);
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
    tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
    cb->setStyleSheet(tmps);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetEqType(int)));
    lyout1->addWidget(cb,0,1,1,1);

    gb = new QGroupBox("Аналоговые");
    gblyout = new QVBoxLayout;
    switch (pc.ModuleBsi.MType1)
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
        tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
        cb->setStyleSheet(tmps);
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
        tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
        cb->setStyleSheet(tmps);
        connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetVoltageClass(int,s_tqComboBox*)));
        gb2lyout->addWidget(cb);
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl = new QLabel("Уставка скачка напряжения для запуска осциллографирования, %");
        gb2lyout->addWidget(lbl);
        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("duosc");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetThreshold(double,s_tqSpinBox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl = new QLabel("Уставка порога мин. уровня для определения частоты, %");
        gb2lyout->addWidget(lbl);
        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("duimin");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(2);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetThreshold(double,s_tqSpinBox*)));
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
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("inom1"+QString::number(i));
            dspbls->setSingleStep(1);
            dspbls->setMinimum(1);
            dspbls->setMaximum(50000);
            dspbls->setDecimals(0);
            dspbls->setAData(i);
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetCurrent(double,s_tqSpinBox*)));
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
        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("duosc");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(0);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetThreshold(double,s_tqSpinBox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl = new QLabel("Уставка скачка тока для запуска осциллографирования, %");
        gb2lyout->addWidget(lbl);
        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("diosc");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(1);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetThreshold(double,s_tqSpinBox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl = new QLabel("Уставка порога мин. уровня для определения частоты, %");
        gb2lyout->addWidget(lbl);
        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("duimin");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(2);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetThreshold(double,s_tqSpinBox*)));
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
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("inom1"+QString::number(i));
            dspbls->setSingleStep(1);
            dspbls->setMinimum(1);
            dspbls->setMaximum(50000);
            dspbls->setDecimals(0);
            dspbls->setAData(i);
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetCurrent(double,s_tqSpinBox*)));
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
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("inom1"+QString::number(i));
            dspbls->setSingleStep(1);
            dspbls->setMinimum(1);
            dspbls->setMaximum(50000);
            dspbls->setDecimals(0);
            dspbls->setAData(i);
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetCurrent(double,s_tqSpinBox*)));
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
        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("diosc");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(1);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetThreshold(double,s_tqSpinBox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl = new QLabel("Уставка порога мин. уровня для определения частоты, %");
        gb2lyout->addWidget(lbl);
        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("duimin");
        dspbls->setSingleStep(0.1);
        dspbls->setMinimum(0.0);
        dspbls->setMaximum(100.0);
        dspbls->setAData(2);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetThreshold(double,s_tqSpinBox*)));
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
        econf->Bci_block.ddosc |= tmpint;
    else
        econf->Bci_block.ddosc &= ~tmpint;
}

void e_confdialog::SetThreshold(double dbl, s_tqSpinBox *ptr)
{
    switch (ptr->getAData().toInt())
    {
    case 0: // % напряжения
    {
        econf->Bci_block.duosc = dbl;
        break;
    }
    case 1:
    {
        econf->Bci_block.diosc = dbl;
        break;
    }
    case 2:
    {
        econf->Bci_block.duimin = dbl;
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
        econf->Bci_block.unom2 = ptr->currentText().toInt();
    else
        econf->Bci_block.unom1 = ptr->currentText().toInt();
}

void e_confdialog::SetCurrent(double dbl, s_tqSpinBox *ptr)
{
    econf->Bci_block.inom1[ptr->getAData().toInt()] = dbl;
}

void e_confdialog::SetSecCurrent(int tmpi, s_tqComboBox *ptr)
{
    Q_UNUSED(tmpi);
    econf->Bci_block.inom2[ptr->getAData().toInt()] = ptr->currentText().toInt();
}

void e_confdialog::SetEqType(int tmpi)
{
    econf->Bci_block.eq_type = tmpi;
}

void e_confdialog::SetNPoints(QString tmpi)
{
    econf->Bci_block.npoints = tmpi.toInt();
}

void e_confdialog::SetNFiltr(int tmpi)
{
    econf->Bci_block.nfiltr = tmpi;
}

void e_confdialog::SetNHFiltr(int tmpi)
{
    econf->Bci_block.nhfiltr = tmpi;
}

void e_confdialog::Set104(double dbl, s_tqSpinBox *ptr)
{
    switch (ptr->getAData().toInt())
    {
    case 0:
    {
        econf->Bci_block.Abs_104=dbl;
        break;
    }
    case 1:
    {
        econf->Bci_block.Cycle_104=dbl;
        break;
    }
    case 2:
    {
        econf->Bci_block.T1_104=dbl;
        break;
    }
    case 3:
    {
        econf->Bci_block.T2_104=dbl;
        break;
    }
    case 4:
    {
        econf->Bci_block.T3_104=dbl;
        break;
    }
    case 5:
    {
        econf->Bci_block.k_104=dbl;
        break;
    }
    case 6:
    {
        econf->Bci_block.w_104=dbl;
        break;
    }
    default:
        break;
    }
}

void e_confdialog::SetCType(int num)
{
    econf->Bci_block.Ctype = num;
}

void e_confdialog::WriteConfDataToModule()
{
    cn->Send(CN_WF, &econf->Bci_block, sizeof(econf->Bci_block), 2, econf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        QMessageBox::information(this,"Успешно!","Операция проведена успешно!");
}

void e_confdialog::SetNewConf()
{
    cn->Send(CN_Cnc);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
    {
        ECONFINFO("Переведено успешно");
        emit BsiIsNeedToBeAcquiredAndChecked();
    }
}

void e_confdialog::SetDefConf()
{
    econf->Bci_defblock.MType = pc.ModuleBsi.MType; // делаем для того, чтобы типы совпадали
    econf->Bci_defblock.MType1 = pc.ModuleBsi.MType1; // делаем для того, чтобы подтипы совпадали
    memcpy(&econf->Bci_block, &econf->Bci_defblock, sizeof(e_config::Bci));
    FillConfData();
}

void e_confdialog::LoadConf()
{
    QByteArray ba;
    ba = pc.LoadFile("Config files (*.ecf)");
    if (pc.RestoreDataMem(&(ba.data()[0]), ba.size(), econf->Config))
    {
        ECONFWARN;
        return;
    }
    FillConfData();
    ECONFINFO("Загрузка прошла успешно!");
}

void e_confdialog::SaveConf()
{
    QByteArray *ba = new QByteArray;
    ba->resize(MAXBYTEARRAY);
    pc.StoreDataMem(&(ba->data()[0]), econf->Config);
    quint32 BaLength = static_cast<quint8>(ba->data()[0]);
    BaLength += static_cast<quint8>(ba->data()[1])*256;
    BaLength += static_cast<quint8>(ba->data()[2])*65536;
    BaLength += static_cast<quint8>(ba->data()[3])*16777216;
    BaLength += 12; // FileHeader
    int res = pc.SaveFile("Config files (*.ecf)", &(ba->data()[0]), BaLength);
    switch (res)
    {
    case 0:
        ECONFINFO("Записано успешно!");
        break;
    case 1:
        ECONFER("Ошибка при записи файла!");
        break;
    case 2:
        ECONFER("Пустое имя файла!");
        break;
    case 3:
        ECONFER("Ошибка открытия файла!");
        break;
    default:
        break;
    }
}
