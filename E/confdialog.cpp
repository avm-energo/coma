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

#include "confdialog.h"
#include "../config.h"

e_confdialog::e_confdialog(QWidget *parent) :
    QDialog(parent)
{
    Config[0] = {BCI_EQTYPE, u32_TYPE, sizeof(quint32), sizeof(Bci_block.eq_type)/sizeof(quint32), &(Bci_block.eq_type)};
    Config[1] = {BCI_NPOINTS, u32_TYPE, sizeof(quint32), sizeof(Bci_block.npoints)/sizeof(quint32), &(Bci_block.npoints)};
    Config[2] = {BCI_NFILTR, u32_TYPE, sizeof(quint32), sizeof(Bci_block.nfiltr)/sizeof(quint32), &Bci_block.nfiltr};
    Config[3] = {BCI_NHFILTR, u32_TYPE, sizeof(quint32), sizeof(Bci_block.nhfiltr)/sizeof(quint32), &Bci_block.nhfiltr};
    Config[4] = {BCI_DDOSC, u32_TYPE, sizeof(quint32), sizeof(Bci_block.ddosc)/sizeof(quint32), &Bci_block.ddosc};
    Config[5] = {BCI_UNOM1, float_TYPE, sizeof(float), sizeof(Bci_block.unom1)/sizeof(float), &Bci_block.unom1};
    Config[6] = {BCI_UNOM2, float_TYPE, sizeof(float), sizeof(Bci_block.unom2)/sizeof(float), &Bci_block.unom2};
    Config[7] = {BCI_INOM1, float_TYPE, sizeof(float), sizeof(Bci_block.inom1)/sizeof(float), &Bci_block.inom1};
    Config[8] = {BCI_INOM2, float_TYPE, sizeof(float), sizeof(Bci_block.inom2)/sizeof(float), &Bci_block.inom2};
    Config[9] = {BCI_DUOSC, float_TYPE, sizeof(float), sizeof(Bci_block.duosc)/sizeof(float), &Bci_block.duosc};
    Config[10] = {BCI_DIOSC, float_TYPE, sizeof(float), sizeof(Bci_block.diosc)/sizeof(float), &Bci_block.diosc};
    Config[11] = {BCI_DUIMIN, float_TYPE, sizeof(float), sizeof(Bci_block.duimin)/sizeof(float), &Bci_block.duimin};
    Config[12] = {BCI_CTYPE, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Ctype)/sizeof(quint32), &Bci_block.Ctype};
    Config[13] = {BCI_ABS_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Abs_104)/sizeof(quint32), &Bci_block.Abs_104};
    Config[14] = {BCI_CYCLE_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Cycle_104)/sizeof(quint32), &Bci_block.Cycle_104};
    Config[15] = {BCI_T1_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T1_104)/sizeof(quint32), &Bci_block.T1_104};
    Config[16] = {BCI_T2_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T2_104)/sizeof(quint32), &Bci_block.T2_104};
    Config[17] = {BCI_T3_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T3_104)/sizeof(quint32), &Bci_block.T3_104};
    Config[18] = {BCI_K_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.k_104)/sizeof(quint32), &Bci_block.k_104};
    Config[19] = {BCI_W_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.w_104)/sizeof(quint32), &Bci_block.w_104};
    Config[20] = {0xFFFF, 0, 0, 0, NULL};
    setAttribute(Qt::WA_DeleteOnClose);
    cn = new canal;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    lyout->addWidget(ConfTW);
    QPushButton *pb1 = new QPushButton("Прочитать конфигурацию из модуля");
    connect(pb1,SIGNAL(clicked()),this,SLOT(GetBci()));
    QPushButton *pb = new QPushButton("Записать конфигурацию в модуль");
    pb->setObjectName("WriteConfPB");
//    pb->setEnabled(false);
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteConfDataToModule()));
    lyout->addWidget(pb1);
    lyout->addWidget(pb);
    pb = new QPushButton("Перейти на новую конфигурацию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetNewConf()));
    lyout->addWidget(pb);
    setLayout(lyout);
    SetupUI();
}

void e_confdialog::GetBci()
{
    connect(cn,SIGNAL(DataReady()),this,SLOT(FillConfData()));
    cn->Send(CN_GF,NULL,0,1,Config);
}

void e_confdialog::FillConfData()
{
    int i;
    disconnect(cn, SIGNAL(DataReady()),this,SLOT(FillConfData()));
    if (cn->result)
    {
        ShowErrMsg(cn->result);
        return;
    }
    QSpinBox *spb;
    s_tqspinbox *dspbls;
    s_tqComboBox *ChTypCB;
    s_tqCheckBox *chb;
    switch (pc.MType1)
    {
    case ET_0T2N: // 2 напряжения, 0 токов
    {
        spb = this->findChild<QSpinBox *>("oscdlyspb");
        if (spb == 0)
            return;
        spb->setValue(Bci_block.oscdly);
        dspbls = this->findChild<s_tqspinbox *>("abs104spb");
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.Abs_104);
        dspbls = this->findChild<s_tqspinbox *>("cycle104spb");
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.Cycle_104);
        dspbls = this->findChild<s_tqspinbox *>("t1104spb");
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.T1_104);
        dspbls = this->findChild<s_tqspinbox *>("t2104spb");
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.T2_104);
        dspbls = this->findChild<s_tqspinbox *>("t3104spb");
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.T3_104);
        dspbls = this->findChild<s_tqspinbox *>("k104spb");
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.k_104);
        dspbls = this->findChild<s_tqspinbox *>("w104spb");
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.w_104);
        ChTypCB = this->findChild<s_tqComboBox *>("ctypecb");
        if (ChTypCB == 0)
            return;
        ChTypCB->setCurrentIndex(Bci_block.Ctype);
        for (i = 0; i < 16; i++)
        {
            ChTypCB = this->findChild<s_tqComboBox *>("chtypcb"+QString::number(i));
            if (ChTypCB == 0)
                return;
            ChTypCB->setCurrentIndex(Bci_block.in_type[i]);
            chb = this->findChild<s_tqCheckBox *>("chb"+QString::number(i));
            if (ChTypCB == 0)
                return;
            if (Bci_block.discosc & (0x0001 << i))
                chb->setChecked(true);
            else
                chb->setChecked(false);
            ChTypCB = this->findChild<s_tqComboBox *>("oscsrccb"+QString::number(i));
            if (ChTypCB == 0)
                return;
            ChTypCB->setCurrentIndex(Bci_block.oscsrc&(0x00000003 << i));

            dspbls = this->findChild<s_tqspinbox *>("inminspb"+QString::number(i));
            if (dspbls == 0)
                return;
            dspbls->setValue(Bci_block.in_min[i]);
            dspbls = this->findChild<s_tqspinbox *>("inmaxspb"+QString::number(i));
            if (dspbls == 0)
                return;
            dspbls->setValue(Bci_block.in_max[i]);
            dspbls = this->findChild<s_tqspinbox *>("invminspb"+QString::number(i));
            if (dspbls == 0)
                return;
            dspbls->setValue(Bci_block.in_vmin[i]);
            dspbls = this->findChild<s_tqspinbox *>("invmaxspb"+QString::number(i));
            if (dspbls == 0)
                return;
            dspbls->setValue(Bci_block.in_vmax[i]);

            dspbls = this->findChild<s_tqspinbox *>("setminminspb"+QString::number(i));
            if (dspbls == 0)
                return;
            dspbls->setValue(Bci_block.setminmin[i]);
            dspbls = this->findChild<s_tqspinbox *>("setminspb"+QString::number(i));
            if (dspbls == 0)
                return;
            dspbls->setValue(Bci_block.setmin[i]);
            dspbls = this->findChild<s_tqspinbox *>("setmaxspb"+QString::number(i));
            if (dspbls == 0)
                return;
            dspbls->setValue(Bci_block.setmax[i]);
            dspbls = this->findChild<s_tqspinbox *>("setmaxmaxspb"+QString::number(i));
            if (dspbls == 0)
                return;
            dspbls->setValue(Bci_block.setmaxmax[i]);
        }
    }
    case ET_1T1N:
    {
        break;
    }
    case ET_2T0N:
    {
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
    QGridLayout *lyout2 = new QGridLayout;
    QGridLayout *lyout3 = new QGridLayout;
    QGridLayout *lyout4 = new QGridLayout;
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
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
    dspbls->setValue(205);
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
    dspbls->setValue(5);
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
    dspbls->setValue(15);
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
    dspbls->setValue(10);
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
    dspbls->setValue(20);
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
    dspbls->setValue(12);
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
    dspbls->setValue(8);
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
    cb->setCurrentIndex(2);
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
    cb->setObjectName("eq_type");
    cb->setModel(cblm);
    cb->setCurrentIndex(1);
    cb->setMinimumWidth(70);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetEqType(int)));
    lyout1->addWidget(cb,0,1,1,1);

    gb = new QGroupBox("Аналоговые");
    gblyout = new QVBoxLayout;
    switch (pc.MType)
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
        cb->setModel(cblm);
        cb->setEditable(true);
        cb->setCurrentIndex(2);
        cb->setAData(0);
        connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetVoltageClass(int,s_tqspinbox*)));
        gb2lyout->addWidget(cb);
        gblyout->addLayout(gb2lyout);

        gb2lyout = new QHBoxLayout;
        lbl=new QLabel("Класс напряжения 2-й группы, кВ:");
        gb2lyout->addWidget(lbl);
        cb = new s_tqComboBox;
        cb->setModel(cblm);
        cb->setEditable(true);
        cb->setCurrentIndex(4);
        cb->setAData(1);
        connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetVoltageClass(int,s_tqspinbox*)));
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
        dspbls->setValue(2.0);
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
        dspbls->setValue(2.0);
        dspbls->setAData(2);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetThreshold(double,s_tqspinbox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);
        gb->setLayout(gblyout);
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
        cb->setModel(cblm);
        cb->setEditable(true);
        cb->setCurrentIndex(2);
        cb->setAData(0);
        connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetVoltageClass(int,s_tqspinbox*)));
        gb2lyout->addWidget(cb);
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные первичные токи ТТ 2-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        for (i = 0; i < 3; i++)
        {
            lbl=new QLabel(QString::number(i+10, 16) + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("inom2"+QString::number(i));
            dspbls->setSingleStep(1);
            dspbls->setValue(1000);
            dspbls->setMinimum(1);
            dspbls->setMaximum(50000);
            dspbls->setAData(i+3);
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetCurrent(double,s_tqspinbox*)));
            gb2lyout->addWidget(dspbls);
        }
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные вторичные токи ТТ 2-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        cbl = QStringList() << "1 А" << "5 А";
        cblm = new QStringListModel;
        cblm->setStringList(cbl);
        for (i = 0; i < 3; i++)
        {
            lbl=new QLabel(QString::number(i+10, 16) + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            cb = new s_tqComboBox;
            cb->setModel(cblm);
            cb->setCurrentIndex(1);
            cb->setAData(3+i);
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
        dspbls->setValue(2.0);
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
        dspbls->setValue(2.0);
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
        dspbls->setValue(2.0);
        dspbls->setAData(2);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetThreshold(double,s_tqspinbox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);
        gb->setLayout(gblyout);
    }
    case ET_2T0N:
    {
        lbl=new QLabel("Номинальные первичные токи ТТ 1-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        for (i = 0; i < 3; i++)
        {
            lbl=new QLabel(QString::number(i+10, 16) + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("inom1"+QString::number(i));
            dspbls->setSingleStep(1);
            dspbls->setValue(600);
            dspbls->setMinimum(1);
            dspbls->setMaximum(50000);
            dspbls->setAData(i);
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetCurrent(double,s_tqspinbox*)));
            gb2lyout->addWidget(dspbls);
        }
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные вторичные токи ТТ 1-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        cbl = QStringList() << "1 А" << "5 А";
        cblm = new QStringListModel;
        cblm->setStringList(cbl);
        for (i = 0; i < 3; i++)
        {
            lbl=new QLabel(QString::number(i+10, 16) + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            cb = new s_tqComboBox;
            cb->setModel(cblm);
            cb->setCurrentIndex(1);
            cb->setAData(i);
            connect(cb,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetSecCurrent(int,s_tqComboBox*)));
            gb2lyout->addWidget(cb);
        }
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные первичные токи ТТ 2-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        for (i = 0; i < 3; i++)
        {
            lbl=new QLabel(QString::number(i+10, 16) + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("inom2"+QString::number(i));
            dspbls->setSingleStep(1);
            dspbls->setValue(1000);
            dspbls->setMinimum(1);
            dspbls->setMaximum(50000);
            dspbls->setAData(i+3);
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetCurrent(double,s_tqspinbox*)));
            gb2lyout->addWidget(dspbls);
        }
        gblyout->addLayout(gb2lyout);

        lbl=new QLabel("Номинальные вторичные токи ТТ 2-й группы, А:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        cbl = QStringList() << "1 А" << "5 А";
        cblm = new QStringListModel;
        cblm->setStringList(cbl);
        for (i = 0; i < 3; i++)
        {
            lbl=new QLabel(QString::number(i+10, 16) + ":"); // A, B, C
            gb2lyout->addWidget(lbl);
            cb = new s_tqComboBox;
            cb->setModel(cblm);
            cb->setCurrentIndex(1);
            cb->setAData(3+i);
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
        dspbls->setValue(2.0);
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
        dspbls->setValue(2.0);
        dspbls->setAData(2);
        connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetThreshold(double,s_tqspinbox*)));
        gb2lyout->addWidget(dspbls);
        gblyout->addLayout(gb2lyout);
        gb->setLayout(gblyout);
    }

        for (i = 0; i < 16; i++)
        {
            QLabel *ChTypL = new QLabel(QString::number(i)+":");
            s_tqComboBox *ChTypCB = new s_tqComboBox;
            ChTypCB->setModel(ChTypSlM);
            ChTypCB->setAData(QVariant(i));
            ChTypCB->setObjectName("chtypcb"+QString::number(i));
            ChTypCB->setCurrentIndex(1);
            connect(ChTypCB,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetChTypData(int,s_tqComboBox*)));
            gb2lyout->addWidget(ChTypL);
            gb2lyout->addWidget(ChTypCB, 1);
            if ((i>1)&&!((i+1)%4))
            {
                gblyout->addLayout(gb2lyout);
                gb2lyout = new QHBoxLayout;
            }
        }
        gb->setLayout(gblyout);
        lyout1->addWidget(gb, 0, 0, 1, 1);
        gb = new QGroupBox("Осциллограммы");
        gblyout = new QVBoxLayout;
        lbl = new QLabel("Каналы записи осциллограмм:");
        gblyout->addWidget(lbl);
        for (i=0; i<16; i++)
        {
            lbl=new QLabel(QString::number(i));
            s_tqCheckBox *chb = new s_tqCheckBox;
            chb->setObjectName("chb"+QString::number(i));
            chb->setText("");
            chb->setAData(QVariant(i));
            chb->setChecked(false);
            connect(chb,SIGNAL(statechanged(int,s_tqCheckBox*)),this,SLOT(SetChOsc(int,s_tqCheckBox*)));
            gb3lyout->addWidget(lbl,0,i,1,1,Qt::AlignCenter);
            gb3lyout->addWidget(chb,1,i,1,1,Qt::AlignCenter);
        }
        gblyout->addLayout(gb3lyout);
        ChTypSl = QStringList() << "Ком. Ц" << "U>" << "DI" << "Любой";
        ChTypSlM = new QStringListModel;
        ChTypSlM->setStringList(ChTypSl);
        lbl = new QLabel("События-инициаторы запуска осциллограмм:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        for (i = 0; i < 16; i++)
        {
            QLabel *ChTypL = new QLabel(QString::number(i)+":");
            s_tqComboBox *ChTypCB = new s_tqComboBox;
            ChTypCB->setObjectName("oscsrccb"+QString::number(i));
            ChTypCB->setModel(ChTypSlM);
            ChTypCB->setAData(QVariant(i));
            ChTypCB->setCurrentIndex(0);
            connect(ChTypCB,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetChOscSrc(int,s_tqComboBox*)));
            gb2lyout->addWidget(ChTypL);
            gb2lyout->addWidget(ChTypCB, 1);
            if ((i>1)&&!((i+1)%4))
            {
                gblyout->addLayout(gb2lyout);
                gb2lyout = new QHBoxLayout;
            }
        }
        gblyout->addLayout(gb2lyout);
        lbl = new QLabel("Задержка в мс начала фиксации максимумов:");
        gblyout->addWidget(lbl);
        spb->setObjectName("oscdlyspb");
        spb->setSingleStep(1);
        spb->setValue(0);
        spb->setMinimum(0);
        spb->setMaximum(10000);
        connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetOscDly(int)));
        gblyout->addWidget(spb);
        gb->setLayout(gblyout);
        lyout1->addWidget(gb, 1, 0, 1, 1);

        gb = new QGroupBox("Диапазоны сигналов");
        gb3lyout = new QGridLayout;
        lbl = new QLabel("№ канала");
        gb3lyout->addWidget(lbl,0,0,1,1,Qt::AlignCenter);
        lbl = new QLabel("Мин. знач.");
        gb3lyout->addWidget(lbl,0,1,1,1,Qt::AlignCenter);
        lbl = new QLabel("Макс. знач.");
        gb3lyout->addWidget(lbl,0,2,1,1,Qt::AlignCenter);
        lbl = new QLabel("Мин. инж.");
        gb3lyout->addWidget(lbl,0,3,1,1,Qt::AlignCenter);
        lbl = new QLabel("Макс. инж.");
        gb3lyout->addWidget(lbl,0,4,1,1,Qt::AlignCenter);
        for (i = 0; i < 16; i++)
        {
            QLabel *ChTypL = new QLabel(QString::number(i));
            gb3lyout->addWidget(ChTypL,i+1,0,1,1,Qt::AlignRight);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("inminspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-20.0);
            dspbls->setMaximum(20.0);
            dspbls->setValue(4);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetInMin(double,s_tqspinbox*)));
            gb3lyout->addWidget(dspbls,i+1,1,1,1,Qt::AlignCenter);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("inmaxspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-20.0);
            dspbls->setMaximum(20.0);
            dspbls->setValue(20);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetInMax(double,s_tqspinbox*)));
            gb3lyout->addWidget(dspbls,i+1,2,1,1,Qt::AlignCenter);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("invminspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-100000.0);
            dspbls->setMaximum(100000.0);
            dspbls->setValue(0);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetInVMin(double,s_tqspinbox*)));
            gb3lyout->addWidget(dspbls,i+1,3,1,1,Qt::AlignCenter);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("invmaxspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-100000.0);
            dspbls->setMaximum(100000.0);
            dspbls->setValue(1000);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetInVMax(double,s_tqspinbox*)));
            gb3lyout->addWidget(dspbls,i+1,4,1,1,Qt::AlignCenter);
        }
        gb->setLayout(gb3lyout);
        lyout2->addWidget(gb, 0, 0, 1, 1);

        gb = new QGroupBox("Уставки");
        gb3lyout = new QGridLayout;
        lbl = new QLabel("№ канала");
        gb3lyout->addWidget(lbl,0,0,1,1,Qt::AlignCenter);
        lbl = new QLabel("Мин. авар.");
        gb3lyout->addWidget(lbl,0,1,1,1,Qt::AlignCenter);
        lbl = new QLabel("Мин. пред.");
        gb3lyout->addWidget(lbl,0,2,1,1,Qt::AlignCenter);
        lbl = new QLabel("Макс. пред.");
        gb3lyout->addWidget(lbl,0,3,1,1,Qt::AlignCenter);
        lbl = new QLabel("Макс. авар.");
        gb3lyout->addWidget(lbl,0,4,1,1,Qt::AlignCenter);
        for (i = 0; i < 16; i++)
        {
            QLabel *ChTypL = new QLabel(QString::number(i));
            gb3lyout->addWidget(ChTypL,i+1,0,1,1,Qt::AlignRight);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("setminminspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-100000.0);
            dspbls->setMaximum(100000.0);
            dspbls->setValue(10);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetMinMin(double,s_tqspinbox*)));
            gb3lyout->addWidget(dspbls,i+1,1,1,1,Qt::AlignCenter);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("setminspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-100000.0);
            dspbls->setMaximum(100000.0);
            dspbls->setValue(50);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetMin(double,s_tqspinbox*)));
            gb3lyout->addWidget(dspbls,i+1,2,1,1,Qt::AlignCenter);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("setmaxspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-100000.0);
            dspbls->setMaximum(100000.0);
            dspbls->setValue(950);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetMax(double,s_tqspinbox*)));
            gb3lyout->addWidget(dspbls,i+1,3,1,1,Qt::AlignCenter);
            dspbls = new s_tqspinbox;
            dspbls->setObjectName("setmaxmaxspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-100000.0);
            dspbls->setMaximum(100000.0);
            dspbls->setValue(990);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqspinbox*)),this,SLOT(SetMaxMax(double,s_tqspinbox*)));
            gb3lyout->addWidget(dspbls,i+1,4,1,1,Qt::AlignCenter);
        }
        gb->setLayout(gb3lyout);
        lyout3->addWidget(gb, 0, 0, 1, 1);

    }
    case MT_C:
    {
        break;
    }
    case MT_D:
    {
        break;
    }
    case MT_E:
    {
        break;
    }
    default:
        break;
    }
    cp1->setLayout(lyout1);
    cp2->setLayout(lyout2);
    cp3->setLayout(lyout3);
    cp4->setLayout(lyout4);
}

void e_confdialog::SetChTypData(int num, s_tqComboBox *cb)
{
    Bci_block.in_type[cb->getAData().toInt()] = num;
}

void e_confdialog::SetOscDly(int dly)
{
    Bci_block.oscdly = dly;
}

void e_confdialog::SetChOsc(int isChecked, s_tqCheckBox *ptr)
{
    quint16 tmpint = 0x0001;
    tmpint = tmpint << ptr->getAData().toInt();
    if (isChecked == Qt::Checked)
        Bci_block.discosc |= tmpint;
    else
        Bci_block.discosc &= ~tmpint;
}

void e_confdialog::SetChOscSrc(int srctyp, s_tqComboBox *ptr)
{
    quint32 tmpint = srctyp << ptr->getAData().toInt();
    quint32 tmpmask = ~(0x00000011 << ptr->getAData().toInt());
    Bci_block.oscsrc &= tmpmask;
    Bci_block.oscsrc |= tmpint;
}

void e_confdialog::SetInMin(double dbl, s_tqspinbox *ptr)
{
    Bci_block.in_min[ptr->getAData().toInt()] = dbl;
}

void e_confdialog::SetInMax(double dbl, s_tqspinbox *ptr)
{
    Bci_block.in_max[ptr->getAData().toInt()] = dbl;
}

void e_confdialog::SetInVMin(double dbl, s_tqspinbox *ptr)
{
    Bci_block.in_vmin[ptr->getAData().toInt()] = dbl;
}

void e_confdialog::SetInVMax(double dbl, s_tqspinbox *ptr)
{
    Bci_block.in_vmax[ptr->getAData().toInt()] = dbl;
}

void e_confdialog::SetMinMin(double dbl, s_tqspinbox *ptr)
{
    Bci_block.setminmin[ptr->getAData().toInt()] = dbl;
}

void e_confdialog::SetMin(double dbl, s_tqspinbox *ptr)
{
    Bci_block.setmin[ptr->getAData().toInt()] = dbl;
}

void e_confdialog::SetMax(double dbl, s_tqspinbox *ptr)
{
    Bci_block.setmax[ptr->getAData().toInt()] = dbl;
}

void e_confdialog::SetMaxMax(double dbl, s_tqspinbox *ptr)
{
    Bci_block.setmaxmax[ptr->getAData().toInt()] = dbl;
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
    QMessageBox::critical(this,"error!",errmsgs.at(ermsg));
}
