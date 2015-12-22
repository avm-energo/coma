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
#include "../widgets/mystackedwidget.h"
#include "a_confdialog.h"
#include "../canal.h"

a_confdialog::a_confdialog(QWidget *parent) :
    QDialog(parent)
{
    NoProperConf = false;
    Config[0] = {ABCI_MTYPE, u32_TYPE, sizeof(quint32), sizeof(Bci_block.MType)/sizeof(quint32), &(Bci_block.MType)};
    Config[1] = {ABCI_MTYPE1, u32_TYPE, sizeof(quint32), sizeof(Bci_block.MType1)/sizeof(quint32), &(Bci_block.MType1)};
    Config[2] = {ABCI_INTYPE, u8_TYPE, sizeof(qint8), sizeof(Bci_block.in_type)/sizeof(qint8), &(Bci_block.in_type)};
    Config[3] = {ABCI_INMIN, float_TYPE, sizeof(float), sizeof(Bci_block.in_min)/sizeof(float), &(Bci_block.in_min)};
    Config[4] = {ABCI_INMAX, float_TYPE, sizeof(float), sizeof(Bci_block.in_max)/sizeof(float), &Bci_block.in_max};
    Config[5] = {ABCI_INVMIN, float_TYPE, sizeof(float), sizeof(Bci_block.in_vmin)/sizeof(float), &Bci_block.in_vmin};
    Config[6] = {ABCI_INVMAX, float_TYPE, sizeof(float), sizeof(Bci_block.in_vmax)/sizeof(float), &Bci_block.in_vmax};
    Config[7] = {ABCI_SETMINMIN, float_TYPE, sizeof(float), sizeof(Bci_block.setminmin)/sizeof(float), &Bci_block.setminmin};
    Config[8] = {ABCI_SETMIN, float_TYPE, sizeof(float), sizeof(Bci_block.setmin)/sizeof(float), &Bci_block.setmin};
    Config[9] = {ABCI_SETMAX, float_TYPE, sizeof(float), sizeof(Bci_block.setmax)/sizeof(float), &Bci_block.setmax};
    Config[10] = {ABCI_SETMAXMAX, float_TYPE, sizeof(float), sizeof(Bci_block.setmaxmax)/sizeof(float), &Bci_block.setmaxmax};
    Config[11] = {ABCI_DISCOSC, u32_TYPE, sizeof(quint32), sizeof(Bci_block.discosc)/sizeof(quint32), &Bci_block.discosc};
    Config[12] = {ABCI_OSCSRC, u32_TYPE, sizeof(quint32), sizeof(Bci_block.oscsrc)/sizeof(quint32), &Bci_block.oscsrc};
    Config[13] = {ABCI_OSCDLY, u16_TYPE, sizeof(quint16), sizeof(Bci_block.oscdly)/sizeof(quint16), &Bci_block.oscdly};
    Config[14] = {ABCI_HYSTERESIS, float_TYPE, sizeof(float), sizeof(Bci_block.hysteresis)/sizeof(float), &Bci_block.hysteresis};
    Config[15] = {ABCI_CTYPE, u16_TYPE, sizeof(quint16), sizeof(Bci_block.Ctype)/sizeof(quint16), &Bci_block.Ctype};
    Config[16] = {ABCI_ABS_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Abs_104)/sizeof(quint32), &Bci_block.Abs_104};
    Config[17] = {ABCI_CYCLE_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Cycle_104)/sizeof(quint32), &Bci_block.Cycle_104};
    Config[18] = {ABCI_T1_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T1_104)/sizeof(quint32), &Bci_block.T1_104};
    Config[19] = {ABCI_T2_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T2_104)/sizeof(quint32), &Bci_block.T2_104};
    Config[20] = {ABCI_T3_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T3_104)/sizeof(quint32), &Bci_block.T3_104};
    Config[21] = {ABCI_K_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.k_104)/sizeof(quint32), &Bci_block.k_104};
    Config[22] = {ABCI_W_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.w_104)/sizeof(quint32), &Bci_block.w_104};
    Config[23] = {0xFFFF, 0, 0, 0, NULL};

    if (pc.MType > 3)
        Bci_defblock.MType = 3;
    else
        Bci_defblock.MType = pc.MType;
    if (pc.MType1 > 0x1000000)
        Bci_defblock.MType1 = 0x810001;
    else
        Bci_defblock.MType1 = pc.MType1;

    Bci_defblock.Abs_104 = 205;
    Bci_defblock.Ctype = 2;
    Bci_defblock.Cycle_104 = 5;
    Bci_defblock.T1_104 = 15;
    Bci_defblock.T2_104 = 10;
    Bci_defblock.T3_104 = 20;
    Bci_defblock.k_104 = 12;
    Bci_defblock.w_104 = 8;
    Bci_defblock.discosc = 0;
    Bci_defblock.oscsrc = 0;
    Bci_defblock.oscdly = 0;
    for (int i = 0; i < 16; i++)
    {
        Bci_defblock.in_type[i] = 1;
        Bci_defblock.in_min[i] = 4;
        Bci_defblock.in_max[i] = 20;
        Bci_defblock.in_vmin[i] = 0;
        Bci_defblock.in_vmax[i] = 1000;
        Bci_defblock.setminmin[i] = 10;
        Bci_defblock.setmin[i] = 50;
        Bci_defblock.setmax[i] = 950;
        Bci_defblock.setmaxmax[i] = 990;
    }

    setAttribute(Qt::WA_DeleteOnClose);
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    lyout->addWidget(ConfTW);
    QWidget *wdgt = new QWidget;
    QGridLayout *wdgtlyout = new QGridLayout;
    QPushButton *pb = new QPushButton("Прочитать из модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(GetBci()));
    if (pc.Emul)
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 0, 1, 1);
    pb = new QPushButton("Записать в модуль");
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
}

void a_confdialog::SetupUI()
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
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);
    cp4->setStyleSheet(tmps);
    QTabWidget *ConfTW = this->findChild<QTabWidget *>("conftw");
    if (ConfTW == 0)
        return;
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
        ChTypCB->setObjectName("chtypcb."+QString::number(i));
        tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
        ChTypCB->setStyleSheet(tmps);
        connect(ChTypCB,SIGNAL(currentIndexChanged(int)),this,SLOT(SetChTypData()));
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
        tmps = "QCheckBox {background-color: "+QString(ACONFGCLR)+";}";
        chb->setStyleSheet(tmps);
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
        tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
        ChTypCB->setStyleSheet(tmps);
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
    tmps = "QSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    spb->setStyleSheet(tmps);
    connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetOscDly(int)));
    gblyout->addWidget(spb);
    gb->setLayout(gblyout);
    lyout1->addWidget(gb, 1, 0, 1, 1);

    gb = new QGroupBox("Диапазоны сигналов");
    ConfLayout = new QGridLayout;
    ConfLayout->setColumnStretch(0,0);
    ConfLayout->setColumnStretch(1,10);
    ConfLayout->setColumnStretch(2,5);
    ConfLayout->setColumnStretch(3,5);
    ConfLayout->setColumnStretch(4,5);
    ConfLayout->setColumnStretch(5,5);
    QHBoxLayout *hlyout = new QHBoxLayout;
    s_tqSpinBox *dspbls;
    lbl = new QLabel("№ канала");
    ConfLayout->addWidget(lbl,0,0,1,1);
    lbl = new QLabel("Тип диапазона");
    ConfLayout->addWidget(lbl,0,1,1,1);
    lbl = new QLabel("Диапазон (мин..макс)");
    ConfLayout->addWidget(lbl,0,2,1,2);
    lbl = new QLabel("Мин. инж.");
    ConfLayout->addWidget(lbl,0,4,1,1);
    lbl = new QLabel("Макс. инж.");
    ConfLayout->addWidget(lbl,0,5,1,1);
    QStringList sl = QStringList() << "Предуст. мА" << "Предуст. В" << "Произвольный";
    QStringListModel *slm = new QStringListModel;
    slm->setStringList(sl);
    for (i = 0; i < 16; i++)
    {
        hlyout = new QHBoxLayout;
        QLabel *ChTypL = new QLabel(QString::number(i));
        ConfLayout->addWidget(ChTypL,i+1,0,1,1);
        s_tqComboBox *mcb = new s_tqComboBox;
        mcb->setObjectName("inrange."+QString::number(i));
        mcb->setModel(slm);
        ConfLayout->addWidget(mcb, i+1,1,1,1);
        connect(mcb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetRangeWidgetSlot(int)));

        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("invmin."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "s_tqSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetInVMin()));
        ConfLayout->addWidget(dspbls,i+1,4,1,1);
        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("invmax."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "s_tqSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetInVMax()));
        ConfLayout->addWidget(dspbls,i+1,5,1,1);
    }
    gb->setLayout(ConfLayout);
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
        dspbls->setObjectName("setminmin."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        dspbls->setAData(QVariant(i));
        tmps = "s_tqSpinBox {background-color: "+QString(ACONFRCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetMinMin()));
        gb3lyout->addWidget(dspbls,i+1,1,1,1,Qt::AlignCenter);
        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("setmin."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        dspbls->setAData(QVariant(i));
        tmps = "s_tqSpinBox {background-color: "+QString(ACONFYCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetMin()));
        gb3lyout->addWidget(dspbls,i+1,2,1,1,Qt::AlignCenter);
        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("setmax."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        dspbls->setAData(QVariant(i));
        tmps = "s_tqSpinBox {background-color: "+QString(ACONFYCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetMax()));
        gb3lyout->addWidget(dspbls,i+1,3,1,1,Qt::AlignCenter);
        dspbls = new s_tqSpinBox;
        dspbls->setObjectName("setmaxmax."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        dspbls->setAData(QVariant(i));
        tmps = "s_tqSpinBox {background-color: "+QString(ACONFRCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetMaxMax()));
        gb3lyout->addWidget(dspbls,i+1,4,1,1,Qt::AlignCenter);
    }
    gb->setLayout(gb3lyout);
    lyout3->addWidget(gb, 0, 0, 1, 1);

    gb = new QGroupBox("Настройки протокола МЭК-60870-5-104");
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
    tmps = "s_tqSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
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
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
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
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
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
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
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
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
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
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
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
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
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
    tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
    ChTypCB->setStyleSheet(tmps);
    connect(ChTypCB,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    gb3lyout->addWidget(ChTypCB, 7, 1, 1, 2);
    gb->setLayout(gb3lyout);
    lyout4->addWidget(gb, 0, 0, 1, 1);
    cp1->setLayout(lyout1);
    cp2->setLayout(lyout2);
    cp3->setLayout(lyout3);
    cp4->setLayout(lyout4);
    SetDefConf();
}

int a_confdialog::GetIndexFromName(QObject *obj)
{
    QStringList tmpsl = obj->objectName().split(".");
    int tmpi = -1;
    if (tmpsl.size() > 1)
        tmpi = tmpsl.at(1).toInt();
    return tmpi;
}

void a_confdialog::SetRangemA()
{
    s_tqComboBox *cb = qobject_cast<s_tqComboBox *>(sender());
    int tmpi;
    if ((tmpi=GetIndexFromName(cb)) == -1)
        return;
    switch(cb->currentIndex())
    {
    case 0: // 4-20 мА
        Bci_block.setmin[tmpi] = 4.0;
        Bci_block.setmax[tmpi] = 20.0;
        break;
    case 1: // -20-20 мА
        Bci_block.setmin[tmpi] = -20.0;
        Bci_block.setmax[tmpi] = 20.0;
        break;
    case 2: // 0-5 мА
        Bci_block.setmin[tmpi] = 0.0;
        Bci_block.setmax[tmpi] = 5.0;
        break;
    default:
        break;
    }
    FillConfData();
}

void a_confdialog::SetRangeV()
{
    s_tqComboBox *cb = qobject_cast<s_tqComboBox *>(sender());
    int tmpi;
    if ((tmpi=GetIndexFromName(cb)) == -1)
        return;
    switch(cb->currentIndex())
    {
    case 0: // 0-5 В
        Bci_block.setmin[tmpi] = 0.0;
        Bci_block.setmax[tmpi] = 5.0;
        break;
    case 1: // -5-5 В
        Bci_block.setmin[tmpi] = -5.0;
        Bci_block.setmax[tmpi] = 5.0;
        break;
    default:
        break;
    }
    FillConfData();
}

void a_confdialog::GetBci()
{
    cn->Send(CN_GF,NULL,0,1,Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        FillConfData();
}

void a_confdialog::FillConfData()
{
    int i;
    QSpinBox *spb;
    s_tqComboBox *cb;
    s_tqCheckBox *chb;
    spb = this->findChild<QSpinBox *>("oscdlyspb");
    if (spb == 0)
    {
        ACONFDBG;
        return;
    }
    spb->setValue(Bci_block.oscdly);
    SetSpinboxValue("abs104spb",Bci_block.Abs_104);
    SetSpinboxValue("cycle104spb",Bci_block.Cycle_104);
    SetSpinboxValue("t1104spb",Bci_block.T1_104);
    SetSpinboxValue("t2104spb",Bci_block.T2_104);
    SetSpinboxValue("t3104spb",Bci_block.T3_104);
    SetSpinboxValue("k104spb",Bci_block.k_104);
    SetSpinboxValue("w104spb",Bci_block.w_104);
    cb = this->findChild<s_tqComboBox *>("ctypecb");
    if (cb == 0)
    {
        ACONFDBG;
        return;
    }
    cb->setCurrentIndex(Bci_block.Ctype);
    for (i = 0; i < 16; i++)
    {
        cb = this->findChild<s_tqComboBox *>("chtypcb."+QString::number(i));
        if (cb == 0)
        {
            ACONFDBG;
            return;
        }
        cb->setCurrentIndex(Bci_block.in_type[i]);
        chb = this->findChild<s_tqCheckBox *>("chb"+QString::number(i));
        if (cb == 0)
        {
            ACONFDBG;
            return;
        }
        if (Bci_block.discosc & (static_cast<quint32>(0x0001) << i))
            chb->setChecked(true);
        else
            chb->setChecked(false);
        cb = this->findChild<s_tqComboBox *>("oscsrccb"+QString::number(i));
        if (cb == 0)
        {
            ACONFDBG;
            return;
        }
        QLabel *lbl = this->findChild<QLabel *>("oscsrcl"+QString::number(i));
        if (lbl == 0)
        {
            ACONFDBG;
            return;
        }
        quint8 tmpi = i << 1;
        cb->setCurrentIndex((Bci_block.oscsrc&(static_cast<quint32>(0x00000003) << tmpi)) >> tmpi);
        if (chb->isChecked())
        {
            cb->setVisible(true);
            lbl->setVisible(true);
        }
        else
        {
            cb->setVisible(false);
            lbl->setVisible(false);
        }
        cb = this->findChild<s_tqComboBox *>("inrange."+QString::number(i));
        if (cb == 0)
        {
            ACONFDBG;
            return;
        }
        switch (Bci_block.in_type[i])
        {
        case INTYPENA: // канал отключён
            break;
        case INTYPEMA: // канал с мА
        {
            if (pc.FloatInRange(Bci_block.in_min[i],0.0) && pc.FloatInRange(Bci_block.in_max[i],20.0))
            {
                cb->setCurrentIndex(RT_mA);
                SetRangeWidget(i, RT_mA);
                QComboBox *c2b = this->findChild<QComboBox *>("rangemA."+QString::number(i));
                if (c2b == 0)
                    return;
                c2b->setCurrentIndex(RT_mA020);
            }
            else if (pc.FloatInRange(Bci_block.in_min[i],4.0) && pc.FloatInRange(Bci_block.in_max[i],20.0))
            {
                cb->setCurrentIndex(RT_mA);
                SetRangeWidget(i, RT_mA);
                QComboBox *c2b = this->findChild<QComboBox *>("rangemA."+QString::number(i));
                if (c2b == 0)
                    return;
                c2b->setCurrentIndex(RT_mA420);
            }
            else if (pc.FloatInRange(Bci_block.in_min[i],0.0) && pc.FloatInRange(Bci_block.in_max[i],5.0))
            {
                cb->setCurrentIndex(RT_mA);
                SetRangeWidget(i, RT_mA);
                QComboBox *c2b = this->findChild<QComboBox *>("rangemA."+QString::number(i));
                if (c2b == 0)
                    return;
                c2b->setCurrentIndex(RT_mA05);
            }
            else
            {
                cb->setCurrentIndex(RT_M);
                SetRangeWidget(i, RT_M);
                SetSpinboxValue("inmin."+QString::number(i), Bci_block.in_min[i]);
                SetSpinboxValue("inmax."+QString::number(i), Bci_block.in_max[i]);
            }
            break;
        }
        case INTYPEV: // канал с В
        {
            if (pc.FloatInRange(Bci_block.in_min[i],0.0) && pc.FloatInRange(Bci_block.in_max[i],5.0))
            {
                cb->setCurrentIndex(RT_V);
                SetRangeWidget(i, RT_V);
                QComboBox *c2b = this->findChild<QComboBox *>("rangeV."+QString::number(i));
                if (c2b == 0)
                    return;
                c2b->setCurrentIndex(RT_V05);
            }
            else if (pc.FloatInRange(Bci_block.in_min[i], -5.0) && pc.FloatInRange(Bci_block.in_max[i],5.0))
            {
                cb->setCurrentIndex(RT_V);
                SetRangeWidget(i, RT_V);
                QComboBox *c2b = this->findChild<QComboBox *>("rangeV."+QString::number(i));
                if (c2b == 0)
                    return;
                c2b->setCurrentIndex(RT_V_55);
            }
            else
            {
                cb->setCurrentIndex(RT_M);
                SetRangeWidget(i, RT_M);
                SetSpinboxValue("inmin."+QString::number(i), Bci_block.in_min[i]);
                SetSpinboxValue("inmax."+QString::number(i), Bci_block.in_max[i]);
            }
            break;
        }
        case INTYPERES:
            break;
        default:
            break;
        }

/*        dspbls = this->findChild<s_tqSpinBox *>("inmin."+QString::number(i));
        if (dspbls == 0)
        {
            ACONFDBG;
            return;
        }
        dspbls->setValue(Bci_block.in_min[i]);
        dspbls = this->findChild<s_tqSpinBox *>("inmax."+QString::number(i));
        if (dspbls == 0)
        {
            ACONFDBG;
            return;
        }
        dspbls->setValue(Bci_block.in_max[i]); */

        SetSpinboxValue("invmin."+QString::number(i), Bci_block.in_vmin[i]);
        SetSpinboxValue("invmax."+QString::number(i), Bci_block.in_vmax[i]);
        SetSpinboxValue("setminmin."+QString::number(i), Bci_block.setminmin[i]);
        SetSpinboxValue("setmin."+QString::number(i), Bci_block.setmin[i]);
        SetSpinboxValue("setmax."+QString::number(i), Bci_block.setmax[i]);
        SetSpinboxValue("setmaxmax."+QString::number(i), Bci_block.setmaxmax[i]);
    }
}

void a_confdialog::SetSpinboxValue(QString name, double value)
{
    s_tqSpinBox *dspbls = this->findChild<s_tqSpinBox *>(name);
    if (dspbls == 0)
    {
        ACONFDBG;
        return;
    }
    dspbls->setValue(value);
}

void a_confdialog::SetChTypData()
{
    QComboBox *cb = qobject_cast<QComboBox *>(sender());
    int tmpi;
    if ((tmpi=GetIndexFromName(cb)) == -1)
        return;
    int value = cb->currentIndex();
    Bci_block.in_type[tmpi] = value;
    DisableChannel(tmpi, (value == 0));
    emit ChannelDisabled(tmpi, (value == 0));
}

void a_confdialog::DisableChannel(int ChNum, bool Disable)
{
    s_tqComboBox *cb;
    s_tqCheckBox *chb = this->findChild<s_tqCheckBox *>("chb"+QString::number(ChNum));
    if (chb != 0)
        chb->setVisible(!Disable);
    cb = this->findChild<s_tqComboBox *>("oscsrccb"+QString::number(ChNum));
    if (cb != 0)
        cb->setVisible(!Disable);
    cb = this->findChild<s_tqComboBox *>("inrange."+QString::number(ChNum));
    if (cb != 0)
        cb->setVisible(!Disable);
    cb = this->findChild<s_tqComboBox *>("rangemA."+QString::number(ChNum));
    if (cb != 0)
        cb->setVisible(!Disable);
    cb = this->findChild<s_tqComboBox *>("rangeV."+QString::number(ChNum));
    if (cb != 0)
        cb->setVisible(!Disable);
    QLabel *lbl = this->findChild<QLabel *>("oscsrcl"+QString::number(ChNum));
    if (lbl != 0)
        lbl->setVisible(!Disable);
    s_tqSpinBox *dspbls = this->findChild<s_tqSpinBox *>("inmin."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<s_tqSpinBox *>("inmax."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<s_tqSpinBox *>("invmin."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<s_tqSpinBox *>("invmax."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<s_tqSpinBox *>("setminmin."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<s_tqSpinBox *>("setmin."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<s_tqSpinBox *>("setmax."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<s_tqSpinBox *>("setmaxmax."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
}

void a_confdialog::SetOscDly(int dly)
{
    Bci_block.oscdly = dly;
}

void a_confdialog::SetChOsc(int isChecked, s_tqCheckBox *ptr)
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

void a_confdialog::SetChOscSrc(int srctyp, s_tqComboBox *ptr)
{
    quint8 tmpi = ptr->getAData().toInt() << 1;
    quint32 tmpint = srctyp << tmpi;
    quint32 tmpmask = ~(0x00000003 << tmpi);
    Bci_block.oscsrc &= tmpmask;
    Bci_block.oscsrc |= tmpint;
}

bool a_confdialog::CheckConf()
{
    bool NotGood = false;
    for (int i=0; i<16; i++)
    {
        if (Bci_block.in_min[i] > Bci_block.in_max[i])
        {
            NotGood = true;
            ACONFER("Минимум не может быть больше максимума, канал "+QString::number(i));
        }
        if (Bci_block.in_vmin[i] > Bci_block.in_vmax[i])
        {
            NotGood = true;
            ACONFER("Инженерный минимум не может быть больше максимума, канал "+QString::number(i));
        }
        if (Bci_block.setminmin[i] > Bci_block.setmin[i])
        {
            NotGood = true;
            ACONFER("Аварийная уставка минимума не может быть больше предупредительной, канал "+QString::number(i));
        }
        if (Bci_block.setminmin[i] > Bci_block.in_vmin[i])
        {
            NotGood = true;
            ACONFER("Уставка не может быть меньше границы диапазона, канал "+QString::number(i));
        }
        if (Bci_block.setmin[i] < Bci_block.in_vmin[i])
        {
            NotGood = true;
            ACONFER("Уставка не может быть меньше границы диапазона, канал "+QString::number(i));
        }
        if (Bci_block.setmax[i] < Bci_block.setmin[i])
        {
            NotGood = true;
            ACONFER("Уставка минимума не может быть больше уставки максимума, канал "+QString::number(i));
        }
        if (Bci_block.setmax[i] > Bci_block.setmaxmax[i])
        {
            NotGood = true;
            ACONFER("Предупредительная уставка максимума не может быть больше аварийной, канал "+QString::number(i));
        }
        if (Bci_block.setmax[i] > Bci_block.in_vmax[i])
        {
            NotGood = true;
            ACONFER("Уставка не может быть больше границы диапазона, канал "+QString::number(i));
        }
        if (Bci_block.setmaxmax[i] > Bci_block.in_vmax[i])
        {
            NotGood = true;
            ACONFER("Уставка не может быть больше границы диапазона, канал "+QString::number(i));
        }
    }
    return NotGood;
}

void a_confdialog::SetInMin()
{
    s_tqSpinBox *spb = qobject_cast<s_tqSpinBox *>(sender());
    int tmpi;
    if ((tmpi=GetIndexFromName(spb)) == -1)
        return;
    Bci_block.in_min[tmpi] = spb->value();
}

void a_confdialog::SetInMax()
{
    s_tqSpinBox *spb = qobject_cast<s_tqSpinBox *>(sender());
    int tmpi;
    if ((tmpi=GetIndexFromName(spb)) == -1)
        return;
    Bci_block.in_max[tmpi] = spb->value();
}

void a_confdialog::SetInVMin()
{
    s_tqSpinBox *spb = qobject_cast<s_tqSpinBox *>(sender());
    int tmpi;
    if ((tmpi=GetIndexFromName(spb)) == -1)
        return;
    Bci_block.in_vmin[tmpi] = spb->value();
}

void a_confdialog::SetInVMax()
{
    s_tqSpinBox *spb = qobject_cast<s_tqSpinBox *>(sender());
    int tmpi;
    if ((tmpi=GetIndexFromName(spb)) == -1)
        return;
    Bci_block.in_vmax[tmpi] = spb->value();
}

void a_confdialog::SetMinMin()
{
    s_tqSpinBox *spb = qobject_cast<s_tqSpinBox *>(sender());
    int tmpi;
    if ((tmpi=GetIndexFromName(spb)) == -1)
        return;
    Bci_block.setminmin[tmpi] = spb->value();
}

void a_confdialog::SetMin()
{
    s_tqSpinBox *spb = qobject_cast<s_tqSpinBox *>(sender());
    int tmpi;
    if ((tmpi=GetIndexFromName(spb)) == -1)
        return;
    Bci_block.setmin[tmpi] = spb->value();
}

void a_confdialog::SetMax()
{
    s_tqSpinBox *spb = qobject_cast<s_tqSpinBox *>(sender());
    int tmpi;
    if ((tmpi=GetIndexFromName(spb)) == -1)
        return;
    Bci_block.setmax[tmpi] = spb->value();
}

void a_confdialog::SetMaxMax()
{
    s_tqSpinBox *spb = qobject_cast<s_tqSpinBox *>(sender());
    int tmpi;
    if ((tmpi=GetIndexFromName(spb)) == -1)
        return;
    Bci_block.setmaxmax[tmpi] = spb->value();
}

void a_confdialog::Set104()
{
    s_tqSpinBox *spb = qobject_cast<s_tqSpinBox *>(sender());
    int tmpi;
    if ((tmpi=GetIndexFromName(spb)) == -1)
        return;
    switch (tmpi)
    {
    case 0:
    {
        Bci_block.Abs_104=spb->value();
        break;
    }
    case 1:
    {
        Bci_block.Cycle_104=spb->value();
        break;
    }
    case 2:
    {
        Bci_block.T1_104=spb->value();
        break;
    }
    case 3:
    {
        Bci_block.T2_104=spb->value();
        break;
    }
    case 4:
    {
        Bci_block.T3_104=spb->value();
        break;
    }
    case 5:
    {
        Bci_block.k_104=spb->value();
        break;
    }
    case 6:
    {
        Bci_block.w_104=spb->value();
        break;
    }
    default:
        break;
    }
}

void a_confdialog::SetCType(int num)
{
    Bci_block.Ctype = num;
}

void a_confdialog::WriteConfDataToModule()
{
    cn->Send(CN_WF, &Bci_block, sizeof(Bci_block), 2, Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        ACONFINFO("Операция проведена успешно!");
}

void a_confdialog::SetNewConf()
{
    cn->Send(CN_Cnc);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        emit BsiIsNeedToBeAcquiredAndChecked();
}

void a_confdialog::SetDefConf()
{
    memcpy(&Bci_block, &Bci_defblock, sizeof(Bci));
    FillConfData();
}

void a_confdialog::UpdateProper(bool tmpb)
{
    NoProperConf = tmpb;
}

void a_confdialog::LoadConf()
{
    QByteArray ba;
    ba = pc.LoadFile("Config files (*.acf)");
    if (pc.RestoreDataMem(&(ba.data()[0]), ba.size(), Config))
    {
        ACONFWARN;
        return;
    }
    FillConfData();
    ACONFINFO("Загрузка прошла успешно!");
}

void a_confdialog::SaveConf()
{
    QByteArray *ba = new QByteArray;
    ba->resize(MAXBYTEARRAY);
    pc.StoreDataMem(&(ba->data()[0]), Config);
    quint32 BaLength = static_cast<quint8>(ba->data()[0]);
    BaLength += static_cast<quint8>(ba->data()[1])*256;
    BaLength += static_cast<quint8>(ba->data()[2])*65536;
    BaLength += static_cast<quint8>(ba->data()[3])*16777216;
    BaLength += 12; // FileHeader
    int res = pc.SaveFile("Config files (*.acf)", &(ba->data()[0]), BaLength);
    switch (res)
    {
    case 0:
        ACONFINFO("Записано успешно!");
        break;
    case 1:
        ACONFER("Ошибка при записи файла!");
        break;
    case 2:
        ACONFER("Пустое имя файла!");
        break;
    case 3:
        ACONFER("Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void a_confdialog::SetRangeWidgetSlot(int RangeType)
{
    QString McbName = sender()->objectName();
    QStringList McbNameSl = McbName.split(".");
    int ChNum;
    if (McbNameSl.size()>1)
        ChNum = McbNameSl.at(1).toInt();
    else
    {
        ACONFWARN;
        return;
    }
    SetRangeWidget(ChNum, RangeType);
}

void a_confdialog::SetRangeWidget(int ChNum, int RangeType)
{
    QLayoutItem *OldWidget = ConfLayout->itemAtPosition(ChNum+1, 2);
    if (OldWidget != 0)
    {
        ConfLayout->removeItem(OldWidget);
        delete OldWidget;
    }
    QHBoxLayout *hlyout = new QHBoxLayout;
    switch (RangeType)
    {
    case RT_mA:
    {
        QComboBox *cb = new QComboBox(this);
        setStyleSheet("QComboBox {border: 1px solid gray; border-radius: 5px;}" \
                      "QComboBox::drop-down {background-color: rgba(100,100,100,255); width: 5px; border-style: none;}");
        cb->addItem("(4..20) мА");
        cb->addItem("(0..20) мА");
        cb->addItem("(0..5) мА");
        cb->setObjectName("rangemA."+QString::number(ChNum));
        connect(cb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetRangemA()));
        hlyout->addWidget(cb);
        break;
    }
    case RT_V:
    {
        QComboBox *cb = new QComboBox(this);
        setStyleSheet("QComboBox {border: 1px solid gray; border-radius: 5px;}" \
                      "QComboBox::drop-down {background-color: rgba(100,100,100,255); width: 5px; border-style: none;}");
        cb->addItem("(0..5) В");
        cb->addItem("(-5..5) В");
        cb->setObjectName("rangeV."+QString::number(ChNum));
        connect(cb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetRangeV()));
        hlyout->addWidget(cb);
        break;
    }
    case RT_M:
    {
        s_tqSpinBox *dspbls = new s_tqSpinBox(this);
        dspbls->setObjectName("inmin."+QString::number(ChNum));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-20.0);
        dspbls->setMaximum(20.0);
        QString tmps = "s_tqSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetInMin()));
        hlyout->addWidget(dspbls);
        dspbls = new s_tqSpinBox(this);
        dspbls->setObjectName("inmax."+QString::number(ChNum));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-20.0);
        dspbls->setMaximum(20.0);
        tmps = "s_tqSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetInMax()));
        hlyout->addWidget(dspbls);
        break;
    }
    default:
        return;
        break;
    }
    ConfLayout->addLayout(hlyout, ChNum+1, 2, 1, 2);
}
