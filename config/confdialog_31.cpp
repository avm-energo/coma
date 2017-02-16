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
#include "confdialog_31.h"
#include "../canal.h"

d_confdialog::d_confdialog(QWidget *parent) :
    QDialog(parent)
{
    NoProperConf = false;
    Config[0] = {DBCI_MTYPE, sizeof(Bci_block.MType), &(Bci_block.MType)};
    Config[1] = {DBCI_MTYPE1, sizeof(Bci_block.MType1), &(Bci_block.MType1)};
    Config[2] = {DBCI_CTYPE, sizeof(Bci_block.Ctype), &Bci_block.Ctype};
    Config[3] = {DBCI_ABS_104, sizeof(Bci_block.Abs_104), &Bci_block.Abs_104};
    Config[4] = {DBCI_CYCLE_104, sizeof(Bci_block.Cycle_104), &Bci_block.Cycle_104};
    Config[5] = {DBCI_T1_104, sizeof(Bci_block.T1_104), &Bci_block.T1_104};
    Config[6] = {DBCI_T2_104, sizeof(Bci_block.T2_104), &Bci_block.T2_104};
    Config[7] = {DBCI_T3_104, sizeof(Bci_block.T3_104), &Bci_block.T3_104};
    Config[8] = {DBCI_K_104, sizeof(Bci_block.k_104), &Bci_block.k_104};
    Config[9] = {DBCI_W_104, sizeof(Bci_block.w_104), &Bci_block.w_104};
    Config[10] = {DBCI_INTYPE, sizeof(Bci_block.in_type), &Bci_block.in_type};
    Config[11] = {DBCI_DLY1, sizeof(Bci_block.dly_time1), &Bci_block.dly_time1};
    Config[12] = {DBCI_DLY2, sizeof(Bci_block.dly_time2), &Bci_block.dly_time2};
    Config[13] = {DBCI_DLY3, sizeof(Bci_block.dly_time3), &Bci_block.dly_time3};
    Config[14] = {DBCI_DLY4, sizeof(Bci_block.dly_time4), &Bci_block.dly_time4};
    Config[15] = {DBCI_PAIR, sizeof(Bci_block.pair), &Bci_block.pair};
    Config[16] = {0xFFFFFFFF, 0, NULL};

    Bci_defblock.MType = 3;
    Bci_defblock.MType1 = 0x810001;
    Bci_defblock.Abs_104 = 205;
    Bci_defblock.Ctype = 2;
    Bci_defblock.Cycle_104 = 5;
    Bci_defblock.T1_104 = 15;
    Bci_defblock.T2_104 = 10;
    Bci_defblock.T3_104 = 20;
    Bci_defblock.k_104 = 12;
    Bci_defblock.w_104 = 8;
    for (int i = 0; i < 16; i++)
    {
        Bci_defblock.in_type[i] = 1;
/*        Bci_defblock.in_min[i] = 4;
        Bci_defblock.in_max[i] = 20;
        Bci_defblock.in_vmin[i] = 0;
        Bci_defblock.in_vmax[i] = 1000;
        Bci_defblock.setminmin[i] = 10;
        Bci_defblock.setmin[i] = 50;
        Bci_defblock.setmax[i] = 950;
        Bci_defblock.setmaxmax[i] = 990; */
    }

    setAttribute(Qt::WA_DeleteOnClose);
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
}

/*void d_confdialog::GetBci()
{
    cn->Send(CN_GF,canal::BT_NONE,NULL,0,1,Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        FillConfData();
}

void d_confdialog::FillConfData()
{
    int i;
    QSpinBox *spb;
    s_tqSpinBox *dspbls;
    s_tqComboBox *ChTypCB;
    s_tqCheckBox *chb;
    spb = this->findChild<QSpinBox *>("oscdlyspb");
    if (spb == 0)
        return;
    spb->setValue(Bci_block.oscdly);
    dspbls = this->findChild<s_tqSpinBox *>("abs104spb");
    if (dspbls == 0)
        return;
    dspbls->setValue(Bci_block.Abs_104);
    dspbls = this->findChild<s_tqSpinBox *>("cycle104spb");
    if (dspbls == 0)
        return;
    dspbls->setValue(Bci_block.Cycle_104);
    dspbls = this->findChild<s_tqSpinBox *>("t1104spb");
    if (dspbls == 0)
        return;
    dspbls->setValue(Bci_block.T1_104);
    dspbls = this->findChild<s_tqSpinBox *>("t2104spb");
    if (dspbls == 0)
        return;
    dspbls->setValue(Bci_block.T2_104);
    dspbls = this->findChild<s_tqSpinBox *>("t3104spb");
    if (dspbls == 0)
        return;
    dspbls->setValue(Bci_block.T3_104);
    dspbls = this->findChild<s_tqSpinBox *>("k104spb");
    if (dspbls == 0)
        return;
    dspbls->setValue(Bci_block.k_104);
    dspbls = this->findChild<s_tqSpinBox *>("w104spb");
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
        if (Bci_block.discosc & (static_cast<quint32>(0x0001) << i))
            chb->setChecked(true);
        else
            chb->setChecked(false);
        ChTypCB = this->findChild<s_tqComboBox *>("oscsrccb"+QString::number(i));
        if (ChTypCB == 0)
            return;
        QLabel *lbl = this->findChild<QLabel *>("oscsrcl"+QString::number(i));
        if (lbl == 0)
            return;
//        quint32 tmpi = static_cast<quint32>(0x00000003) << i;
        quint8 tmpi = i << 1;
        ChTypCB->setCurrentIndex((Bci_block.oscsrc&(static_cast<quint32>(0x00000003) << tmpi)) >> tmpi);
        if (chb->isChecked())
        {
            ChTypCB->setVisible(true);
            lbl->setVisible(true);
        }
        else
        {
            ChTypCB->setVisible(false);
            lbl->setVisible(false);
        }
        dspbls = this->findChild<s_tqSpinBox *>("inminspb"+QString::number(i));
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.in_min[i]);
        dspbls = this->findChild<s_tqSpinBox *>("inmaxspb"+QString::number(i));
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.in_max[i]);
        dspbls = this->findChild<s_tqSpinBox *>("invminspb"+QString::number(i));
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.in_vmin[i]);
        dspbls = this->findChild<s_tqSpinBox *>("invmaxspb"+QString::number(i));
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.in_vmax[i]);

        dspbls = this->findChild<s_tqSpinBox *>("setminminspb"+QString::number(i));
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.setminmin[i]);
        dspbls = this->findChild<s_tqSpinBox *>("setminspb"+QString::number(i));
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.setmin[i]);
        dspbls = this->findChild<s_tqSpinBox *>("setmaxspb"+QString::number(i));
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.setmax[i]);
        dspbls = this->findChild<s_tqSpinBox *>("setmaxmaxspb"+QString::number(i));
        if (dspbls == 0)
            return;
        dspbls->setValue(Bci_block.setmaxmax[i]);
    }
} */

void d_confdialog::SetupUI()
{
/*    int i;
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
    switch (pc.ModuleBsi.MType)
    {
    case MT_A:
    {
        ConfTW->addTab(cp1,"Общие");
        ConfTW->addTab(cp2,"Диапазоны");
        ConfTW->addTab(cp3,"Уставки");
        ConfTW->addTab(cp4,"104");
        QGroupBox *gb = new QGroupBox("Типы каналов");
        QVBoxLayout *gblyout = new QVBoxLayout;
        QHBoxLayout *gb2lyout = new QHBoxLayout;
        QStringList ChTypSl = QStringList() << "Не исп." << "мА" << "В";
        QStringListModel *ChTypSlM = new QStringListModel;
        ChTypSlM->setStringList(ChTypSl);
        for (i = 0; i < 16; i++)
        {
            QLabel *ChTypL = new QLabel(QString::number(i)+":");
            s_tqComboBox *ChTypCB = new s_tqComboBox;
            ChTypCB->setModel(ChTypSlM);
            ChTypCB->setAData(QVariant(i));
            ChTypCB->setObjectName("chtypcb"+QString::number(i));
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
        QGridLayout *gb3lyout = new QGridLayout;
        QLabel *lbl = new QLabel("Каналы записи осциллограмм:");
        gblyout->addWidget(lbl);
        for (i=0; i<16; i++)
        {
            lbl=new QLabel(QString::number(i));
            s_tqCheckBox *chb = new s_tqCheckBox;
            chb->setObjectName("chb"+QString::number(i));
            chb->setText("");
            chb->setAData(QVariant(i));
            connect(chb,SIGNAL(statechanged(int,s_tqCheckBox*)),this,SLOT(SetChOsc(int,s_tqCheckBox*)));
            gb3lyout->addWidget(lbl,0,i,1,1,Qt::AlignCenter);
            gb3lyout->addWidget(chb,1,i,1,1,Qt::AlignCenter);
        }
        gblyout->addLayout(gb3lyout);
        ChTypSl = QStringList() << "Ком. Ц" << "Ц+U>" << "Ц+DI" << "Любой";
        ChTypSlM = new QStringListModel;
        ChTypSlM->setStringList(ChTypSl);
        lbl = new QLabel("События-инициаторы запуска осциллограмм:");
        gblyout->addWidget(lbl);
        gb2lyout = new QHBoxLayout;
        for (i = 0; i < 16; i++)
        {
            QLabel *ChTypL = new QLabel(QString::number(i)+":");
            ChTypL->setObjectName("oscsrcl"+QString::number(i));
            s_tqComboBox *ChTypCB = new s_tqComboBox;
            ChTypCB->setObjectName("oscsrccb"+QString::number(i));
            ChTypCB->setModel(ChTypSlM);
            ChTypCB->setAData(QVariant(i));
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
        QSpinBox *spb = new QSpinBox;
        spb->setObjectName("oscdlyspb");
        spb->setSingleStep(1);
        spb->setMinimum(0);
        spb->setMaximum(10000);
        connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetOscDly(int)));
        gblyout->addWidget(spb);
        gb->setLayout(gblyout);
        lyout1->addWidget(gb, 1, 0, 1, 1);

        gb = new QGroupBox("Диапазоны сигналов");
        gb3lyout = new QGridLayout;
        s_tqSpinBox *dspbls;
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
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("inminspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-20.0);
            dspbls->setMaximum(20.0);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetInMin(double,s_tqSpinBox*)));
            gb3lyout->addWidget(dspbls,i+1,1,1,1,Qt::AlignCenter);
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("inmaxspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-20.0);
            dspbls->setMaximum(20.0);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetInMax(double,s_tqSpinBox*)));
            gb3lyout->addWidget(dspbls,i+1,2,1,1,Qt::AlignCenter);
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("invminspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-100000.0);
            dspbls->setMaximum(100000.0);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetInVMin(double,s_tqSpinBox*)));
            gb3lyout->addWidget(dspbls,i+1,3,1,1,Qt::AlignCenter);
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("invmaxspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-100000.0);
            dspbls->setMaximum(100000.0);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetInVMax(double,s_tqSpinBox*)));
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
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("setminminspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-100000.0);
            dspbls->setMaximum(100000.0);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetMinMin(double,s_tqSpinBox*)));
            gb3lyout->addWidget(dspbls,i+1,1,1,1,Qt::AlignCenter);
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("setminspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-100000.0);
            dspbls->setMaximum(100000.0);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetMin(double,s_tqSpinBox*)));
            gb3lyout->addWidget(dspbls,i+1,2,1,1,Qt::AlignCenter);
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("setmaxspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-100000.0);
            dspbls->setMaximum(100000.0);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetMax(double,s_tqSpinBox*)));
            gb3lyout->addWidget(dspbls,i+1,3,1,1,Qt::AlignCenter);
            dspbls = new s_tqSpinBox;
            dspbls->setObjectName("setmaxmaxspb"+QString::number(i));
            dspbls->setSingleStep(0.01);
            dspbls->setMinimum(-100000.0);
            dspbls->setMaximum(100000.0);
            dspbls->setAData(QVariant(i));
            connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(SetMaxMax(double,s_tqSpinBox*)));
            gb3lyout->addWidget(dspbls,i+1,4,1,1,Qt::AlignCenter);
        }
        gb->setLayout(gb3lyout);
        lyout3->addWidget(gb, 0, 0, 1, 1);

        gb = new QGroupBox("Настройки протокола МЭК-60870-5-104");
        gb3lyout = new QGridLayout;
//        gb3lyout->setColumnStretch(0, 0);
//        gb3lyout->setColumnStretch(1, 90);
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
        connect(dspbls,SIGNAL(valueChanged(double,s_tqSpinBox*)),this,SLOT(Set104(double,s_tqSpinBox*)));
        gb3lyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);
        lbl=new QLabel("c");
        gb3lyout->addWidget(lbl,6,2,1,1);
        lbl = new QLabel("Тип синхр. времени:");
        gb3lyout->addWidget(lbl,7,0,1,1,Qt::AlignRight);
        s_tqComboBox *ChTypCB = new s_tqComboBox;
        ChTypCB->setObjectName("ctypecb");
        ChTypSl = QStringList() << "SNTPIP2+PPS" << "SNTPIP1+PPS" << "SNTPIP1";
        ChTypSlM = new QStringListModel;
        ChTypSlM->setStringList(ChTypSl);
        ChTypCB->setModel(ChTypSlM);
        ChTypCB->setMinimumWidth(70);
        connect(ChTypCB,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
        gb3lyout->addWidget(ChTypCB, 7, 1, 1, 2);
        gb->setLayout(gb3lyout);
        lyout4->addWidget(gb, 0, 0, 1, 1);
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
    SetDefConf();*/
}
/*
void d_confdialog::SetChTypData(int num, s_tqComboBox *cb)
{
    Bci_block.in_type[cb->getAData().toInt()] = num;
}

void d_confdialog::SetOscDly(int dly)
{
    Bci_block.oscdly = dly;
}

void d_confdialog::SetChOsc(int isChecked, s_tqCheckBox *ptr)
{
    quint16 tmpint = 0x0001;
    tmpint = tmpint << ptr->getAData().toInt();
    s_tqComboBox *cb = this->findChild<s_tqComboBox *>("oscsrccb"+QString::number(ptr->getAData().toInt()));
    QLabel *lbl = this->findChild<QLabel *>("oscsrcl"+QString::number(ptr->getAData().toInt()));
    if (isChecked == Qt::Checked)
    {
        Bci_block.discosc |= tmpint;
        if (cb != 0)
            cb->setVisible(true);
        if (lbl != 0)
            lbl->setVisible(true);
    }
    else
    {
        Bci_block.discosc &= ~tmpint;
        if (cb != 0)
            cb->setVisible(false);
        if (lbl != 0)
            lbl->setVisible(false);
    }
}

void d_confdialog::SetChOscSrc(int srctyp, s_tqComboBox *ptr)
{
    quint8 tmpi = ptr->getAData().toInt() << 1;
    quint32 tmpint = srctyp << tmpi;
    quint32 tmpmask = ~(0x00000003 << tmpi);
    Bci_block.oscsrc &= tmpmask;
    Bci_block.oscsrc |= tmpint;
}

void d_confdialog::SetInMin(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.in_min[ptr->getAData().toInt()] = dbl;
}

void d_confdialog::SetInMax(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.in_max[ptr->getAData().toInt()] = dbl;
}

void d_confdialog::SetInVMin(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.in_vmin[ptr->getAData().toInt()] = dbl;
}

void d_confdialog::SetInVMax(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.in_vmax[ptr->getAData().toInt()] = dbl;
}

void d_confdialog::SetMinMin(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.setminmin[ptr->getAData().toInt()] = dbl;
}

void d_confdialog::SetMin(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.setmin[ptr->getAData().toInt()] = dbl;
}

void d_confdialog::SetMax(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.setmax[ptr->getAData().toInt()] = dbl;
}

void d_confdialog::SetMaxMax(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.setmaxmax[ptr->getAData().toInt()] = dbl;
}

void d_confdialog::Set104(double dbl, s_tqSpinBox *ptr)
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

void d_confdialog::SetCType(int num)
{
    Bci_block.Ctype = num;
}

void d_confdialog::WriteConfDataToModule()
{
    connect(cn,SIGNAL(DataReady()),this,SLOT(WriteCompleted()));
    cn->Send(CN_WF, canal::BT_NONE, &Bci_block, sizeof(Bci_block), 2, Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        QMessageBox::information(this,"Успешно!","Операция проведена успешно!");
}

void d_confdialog::SetNewConf()
{
    cn->Send(CN_Cnc);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        emit BsiIsNeedToBeAcquiredAndChecked();
}

void d_confdialog::SetDefConf()
{
    memcpy(&Bci_block, &Bci_defblock, sizeof(Bci));
    FillConfData();
}

void d_confdialog::UpdateProper(bool tmpb)
{
    NoProperConf = tmpb;
} */
