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
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QTabBar>
#include "a_confdialog.h"
#include "../canal.h"

a_confdialog::a_confdialog(QWidget *parent) :
    QDialog(parent)
{
    ChTypModelIsFilling = false;
    aconf = new a_config;
    NoProperConf = false;

    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    if ((pc.ModuleBsi.Hth & HTH_CONFIG) || (pc.Emul)) // если в модуле нет конфигурации, заполнить поля по умолчанию
        SetDefConf();
    else // иначе заполнить значениями из модуля
        GetBci();
}

void a_confdialog::SetupUI()
{
    int i;
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);

    QVBoxLayout *lyout1 = new QVBoxLayout;
    QVBoxLayout *lyout2 = new QVBoxLayout;
    QVBoxLayout *lyout3 = new QVBoxLayout;
    QVBoxLayout *lyout4 = new QVBoxLayout;
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QWidget *cp4 = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);
    cp4->setStyleSheet(tmps);
    QGroupBox *gb = new QGroupBox("Типы каналов");
    QVBoxLayout *gblyout = new QVBoxLayout;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QStringList ChTypSl = QStringList() << "Не исп." << "мА" << "В";
    QStringListModel *ChTypSlM = new QStringListModel;
    ChTypSlM->setStringList(ChTypSl);
    for (i = 0; i < 16; i++)
    {
        QLabel *ChTypL = new QLabel(QString::number(i)+":");
        QComboBox *ChTypCB = new QComboBox;
        ChTypCB->setModel(ChTypSlM);
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
    lyout1->addWidget(gb);
    gb = new QGroupBox("Осциллограммы");
    gblyout = new QVBoxLayout;
    QGridLayout *gb3lyout = new QGridLayout;
    QLabel *lbl = new QLabel("Каналы записи осциллограмм:");
    gblyout->addWidget(lbl);
    for (i=0; i<16; i++)
    {
        lbl=new QLabel(QString::number(i));
        QCheckBox *chb = new QCheckBox;
        chb->setObjectName("chb."+QString::number(i));
        chb->setText("");
        tmps = "QCheckBox {background-color: "+QString(ACONFGCLR)+";}";
        chb->setStyleSheet(tmps);
        connect(chb,SIGNAL(stateChanged(int)),this,SLOT(SetChOsc(int)));
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
        ChTypL->setObjectName("oscsrcl."+QString::number(i));
        QComboBox *ChTypCB = new QComboBox;
        ChTypCB->setObjectName("oscsrccb."+QString::number(i));
        ChTypCB->setModel(ChTypSlM);
        tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
        ChTypCB->setStyleSheet(tmps);
        connect(ChTypCB,SIGNAL(currentIndexChanged(int)),this,SLOT(SetChOscSrc(int)));
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
    lyout1->addWidget(gb);

    gb = new QGroupBox("Диапазоны сигналов");
    gb->setObjectName("RangeGB");
    QGridLayout *rlyout = new QGridLayout;
    rlyout->setColumnStretch(0,0);
    rlyout->setColumnStretch(1,10);
    rlyout->setColumnStretch(2,5);
    rlyout->setColumnStretch(3,5);
    rlyout->setColumnStretch(4,5);
    rlyout->setColumnStretch(5,5);
    QDoubleSpinBox *dspbls;
    lbl = new QLabel("№ канала");
    rlyout->addWidget(lbl,0,0,1,1);
    lbl = new QLabel("Тип диапазона");
    rlyout->addWidget(lbl,0,1,1,1);
    lbl = new QLabel("Диапазон");
    rlyout->addWidget(lbl,0,2,1,2);
    lbl = new QLabel("Мин. инж.");
    rlyout->addWidget(lbl,0,3,1,1);
    lbl = new QLabel("Макс. инж.");
    rlyout->addWidget(lbl,0,4,1,1);
    QStringList sl = QStringList() << "Предуст. мА" << "Произвольный";
    QStringListModel *slm = new QStringListModel;
    slm->setStringList(sl);
    for (i = 0; i < 16; i++)
    {
        QLabel *ChTypL = new QLabel(QString::number(i));
        rlyout->addWidget(ChTypL,i+1,0,1,1);
        QComboBox *mcb = new QComboBox;
        mcb->setObjectName("inrange."+QString::number(i));
        mcb->setModel(slm);
        rlyout->addWidget(mcb, i+1,1,1,1);
        connect(mcb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetRangeWidgetSlot(QString)));
        dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("invmin."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        rlyout->addWidget(dspbls,i+1,3,1,1);
        dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("invmax."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        rlyout->addWidget(dspbls,i+1,4,1,1);
    }
    gb->setLayout(rlyout);
    lyout2->addWidget(gb);

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
        dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("setminmin."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFRCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        gb3lyout->addWidget(dspbls,i+1,1,1,1,Qt::AlignCenter);
        dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("setmin."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFYCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        gb3lyout->addWidget(dspbls,i+1,2,1,1,Qt::AlignCenter);
        dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("setmax."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFYCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        gb3lyout->addWidget(dspbls,i+1,3,1,1,Qt::AlignCenter);
        dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("setmaxmax."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFRCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        gb3lyout->addWidget(dspbls,i+1,4,1,1,Qt::AlignCenter);
    }
    gb->setLayout(gb3lyout);
    lyout3->addWidget(gb);

    gb = new QGroupBox("Настройки протокола МЭК-60870-5-104");
    gb3lyout = new QGridLayout;
    gb3lyout->setColumnStretch(2, 50);
    lbl = new QLabel("Адрес базовой станции:");
    gb3lyout->addWidget(lbl,0,0,1,1,Qt::AlignRight);
    dspbls = new QDoubleSpinBox;
    dspbls->setObjectName("abs104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(65535);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
    gb3lyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);
    lbl = new QLabel("Интервал циклического опроса:");
    gb3lyout->addWidget(lbl,1,0,1,1,Qt::AlignRight);
    dspbls = new QDoubleSpinBox;
    dspbls->setObjectName("cycle104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
    gb3lyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,1,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t1:");
    gb3lyout->addWidget(lbl,2,0,1,1,Qt::AlignRight);
    dspbls = new QDoubleSpinBox;
    dspbls->setObjectName("t1104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
    gb3lyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,2,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t2:");
    gb3lyout->addWidget(lbl,3,0,1,1,Qt::AlignRight);
    dspbls = new QDoubleSpinBox;
    dspbls->setObjectName("t2104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
    gb3lyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,3,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t3:");
    gb3lyout->addWidget(lbl,4,0,1,1,Qt::AlignRight);
    dspbls = new QDoubleSpinBox;
    dspbls->setObjectName("t3104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
    gb3lyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,4,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    gb3lyout->addWidget(lbl,5,0,1,1,Qt::AlignRight);
    dspbls = new QDoubleSpinBox;
    dspbls->setObjectName("k104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
    gb3lyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,5,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    gb3lyout->addWidget(lbl,6,0,1,1,Qt::AlignRight);
    dspbls = new QDoubleSpinBox;
    dspbls->setObjectName("w104spb");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(editingFinished()),this,SLOT(Set104()));
    gb3lyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,6,2,1,1);
    lbl = new QLabel("Тип синхр. времени:");
    gb3lyout->addWidget(lbl,7,0,1,1,Qt::AlignRight);
    QComboBox *ChTypCB = new QComboBox;
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
    lyout4->addWidget(gb);
    cp1->setLayout(lyout1);
    cp2->setLayout(lyout2);
    cp3->setLayout(lyout3);
    cp4->setLayout(lyout4);

    ConfTW->addTab(cp1,"Общие");
    ConfTW->addTab(cp2,"Диапазоны");
    ConfTW->addTab(cp3,"Уставки");
    ConfTW->addTab(cp4,"104");
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
}

void a_confdialog::SetRangemA()
{
    QComboBox *cb = qobject_cast<QComboBox *>(sender());
    if (cb == 0)
        return;
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == -1)
        return;
    SetRange(cb->currentIndex(), tmpi);
}

void a_confdialog::SetRangeV()
{
    QComboBox *cb = qobject_cast<QComboBox *>(sender());
    if (cb == 0)
        return;
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == -1)
        return;
    SetRange(cb->currentIndex()+RT_V05, tmpi); // перемещаем диапазон комбобокса в область V
}

void a_confdialog::SetRange(int Range, int ChNum)
{
    switch(Range)
    {
    case RT_mA420: // 4-20 мА
        aconf->Bci_block.in_min[ChNum] = 4.0;
        aconf->Bci_block.in_max[ChNum] = 20.0;
        break;
    case RT_mA020: // -20-20 мА
        aconf->Bci_block.in_min[ChNum] = 0.0;
        aconf->Bci_block.in_max[ChNum] = 20.0;
        break;
    case RT_mA05: // 0-5 мА
        aconf->Bci_block.in_min[ChNum] = 0.0;
        aconf->Bci_block.in_max[ChNum] = 5.0;
        break;
    case RT_V05: // 0-5 В
        aconf->Bci_block.in_min[ChNum] = 0.0;
        aconf->Bci_block.in_max[ChNum] = 5.0;
        break;
    case RT_V_55: // -5-5 В
        aconf->Bci_block.in_min[ChNum] = -5.0;
        aconf->Bci_block.in_max[ChNum] = 5.0;
        break;
    default:
        break;
    }
}

void a_confdialog::GetBci()
{
    cn->Send(CN_GF,NULL,0,1,aconf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        FillConfData();
}

void a_confdialog::FillConfData()
{
    int i;
    QSpinBox *spb;
    QComboBox *cb;
    QCheckBox *chb;
    spb = this->findChild<QSpinBox *>("oscdlyspb");
    if (spb == 0)
    {
        ACONFDBG;
        return;
    }
    spb->setValue(aconf->Bci_block.oscdly);
    SetSpinboxValue("abs104spb",aconf->Bci_block.Abs_104);
    SetSpinboxValue("cycle104spb",aconf->Bci_block.Cycle_104);
    SetSpinboxValue("t1104spb",aconf->Bci_block.T1_104);
    SetSpinboxValue("t2104spb",aconf->Bci_block.T2_104);
    SetSpinboxValue("t3104spb",aconf->Bci_block.T3_104);
    SetSpinboxValue("k104spb",aconf->Bci_block.k_104);
    SetSpinboxValue("w104spb",aconf->Bci_block.w_104);
    cb = this->findChild<QComboBox *>("ctypecb");
    if (cb == 0)
    {
        ACONFDBG;
        return;
    }
    cb->setCurrentIndex(aconf->Bci_block.Ctype);
    for (i = 0; i < 16; i++)
    {
        cb = this->findChild<QComboBox *>("chtypcb."+QString::number(i));
        if (cb == 0)
        {
            ACONFDBG;
            return;
        }
        cb->setCurrentIndex(aconf->Bci_block.in_type[i]);
        chb = this->findChild<QCheckBox *>("chb."+QString::number(i));
        if (cb == 0)
        {
            ACONFDBG;
            return;
        }
        if (aconf->Bci_block.discosc & (static_cast<quint32>(0x0001) << i))
            chb->setChecked(true);
        else
            chb->setChecked(false);
        cb = this->findChild<QComboBox *>("oscsrccb."+QString::number(i));
        if (cb == 0)
        {
            ACONFDBG;
            return;
        }
        QLabel *lbl = this->findChild<QLabel *>("oscsrcl."+QString::number(i));
        if (lbl == 0)
        {
            ACONFDBG;
            return;
        }
        quint8 tmpi = i << 1;
        cb->setCurrentIndex((aconf->Bci_block.oscsrc&(static_cast<quint32>(0x00000003) << tmpi)) >> tmpi);
        SetMinMax(i);
        SetSpinboxValue("invmin."+QString::number(i), aconf->Bci_block.in_vmin[i]);
        SetSpinboxValue("invmax."+QString::number(i), aconf->Bci_block.in_vmax[i]);
        SetSpinboxValue("setminmin."+QString::number(i), aconf->Bci_block.setminmin[i]);
        SetSpinboxValue("setmin."+QString::number(i), aconf->Bci_block.setmin[i]);
        SetSpinboxValue("setmax."+QString::number(i), aconf->Bci_block.setmax[i]);
        SetSpinboxValue("setmaxmax."+QString::number(i), aconf->Bci_block.setmaxmax[i]);
        DisableChannel(i, (aconf->Bci_block.in_type[i] == INTYPENA));
    }
}

void a_confdialog::SetMinMax(int i)
{
    QComboBox *cb = this->findChild<QComboBox *>("inrange."+QString::number(i));
    if (cb == 0)
    {
        ACONFDBG;
        return;
    }
    switch (aconf->Bci_block.in_type[i])
    {
    case INTYPEMA: // канал с мА
    {
        if (pc.FloatInRange(aconf->Bci_block.in_min[i],0.0) && pc.FloatInRange(aconf->Bci_block.in_max[i],20.0))
        {
            cb->setCurrentText(RT_mAText);
            SetRangeWidget(i, RT_mA); // принудительный вызов, чтобы c2b не было равно 0
            QComboBox *c2b = this->findChild<QComboBox *>("rangemA."+QString::number(i));
            if (c2b == 0)
                return;
            c2b->setCurrentIndex(RT_mA020);
        }
        else if (pc.FloatInRange(aconf->Bci_block.in_min[i],4.0) && pc.FloatInRange(aconf->Bci_block.in_max[i],20.0))
        {
            cb->setCurrentText(RT_mAText);
            SetRangeWidget(i, RT_mA); // принудительный вызов, чтобы c2b не было равно 0
            QComboBox *c2b = this->findChild<QComboBox *>("rangemA."+QString::number(i));
            if (c2b == 0)
                return;
            c2b->setCurrentIndex(RT_mA420);
        }
        else if (pc.FloatInRange(aconf->Bci_block.in_min[i],0.0) && pc.FloatInRange(aconf->Bci_block.in_max[i],5.0))
        {
            cb->setCurrentText(RT_mAText);
            SetRangeWidget(i, RT_mA); // принудительный вызов, чтобы c2b не было равно 0
            QComboBox *c2b = this->findChild<QComboBox *>("rangemA."+QString::number(i));
            if (c2b == 0)
                return;
            c2b->setCurrentIndex(RT_mA05);
        }
        else
        {
            cb->setCurrentText(RT_MText);
            SetRangeWidget(i, RT_M); // принудительный вызов, чтобы inmin не было равно 0
        }
        break;
    }
    case INTYPEV: // канал с В
    {
        if (pc.FloatInRange(aconf->Bci_block.in_min[i],0.0) && pc.FloatInRange(aconf->Bci_block.in_max[i],5.0))
        {
            cb->setCurrentText(RT_VText);
            SetRangeWidget(i, RT_V); // принудительный вызов, чтобы c2b не было равно 0
            QComboBox *c2b = this->findChild<QComboBox *>("rangeV."+QString::number(i));
            if (c2b == 0)
                return;
            c2b->setCurrentIndex(RT_V05-3);
        }
        else if (pc.FloatInRange(aconf->Bci_block.in_min[i], -5.0) && pc.FloatInRange(aconf->Bci_block.in_max[i],5.0))
        {
            cb->setCurrentText(RT_VText);
            SetRangeWidget(i, RT_V); // принудительный вызов, чтобы c2b не было равно 0
            QComboBox *c2b = this->findChild<QComboBox *>("rangeV."+QString::number(i));
            if (c2b == 0)
                return;
            c2b->setCurrentIndex(RT_V_55-3);
        }
        else
        {
            cb->setCurrentText(RT_MText);
            SetRangeWidget(i, RT_M); // принудительный вызов, чтобы inmin не было равно 0
        }
        break;
    }
    default: // INTYPENA, INTYPERES
        break;
    }
}

void a_confdialog::SetSpinboxValue(QString name, double value)
{
    QDoubleSpinBox *dspbls = this->findChild<QDoubleSpinBox *>(name);
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
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == -1)
        return;
    int value = cb->currentIndex();
    aconf->Bci_block.in_type[tmpi] = value;
    DisableChannel(tmpi, (value == 0));
    ChTypModelIsFilling = true;
    SetRangeCB(tmpi, value);
    ChTypModelIsFilling = false;
}

void a_confdialog::DisableChannel(int ChNum, bool Disable)
{
    QComboBox *cb;
    QCheckBox *chb = this->findChild<QCheckBox *>("chb."+QString::number(ChNum));
    if (chb != 0)
        chb->setVisible(!Disable);
    cb = this->findChild<QComboBox *>("oscsrccb."+QString::number(ChNum));
    if (cb != 0)
        cb->setVisible(!Disable && (chb->isChecked()));
    QLabel *lbl = this->findChild<QLabel *>("oscsrcl."+QString::number(ChNum));
    if (lbl != 0)
        lbl->setVisible(!Disable && (chb->isChecked()));
    cb = this->findChild<QComboBox *>("inrange."+QString::number(ChNum));
    if (cb != 0)
        cb->setVisible(!Disable);
    cb = this->findChild<QComboBox *>("rangemA."+QString::number(ChNum));
    if (cb != 0)
        cb->setVisible(!Disable);
    cb = this->findChild<QComboBox *>("rangeV."+QString::number(ChNum));
    if (cb != 0)
        cb->setVisible(!Disable);
    QDoubleSpinBox *dspbls = this->findChild<QDoubleSpinBox *>("inmin."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("inmax."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("invmin."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("invmax."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("setminmin."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("setmin."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("setmax."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("setmaxmax."+QString::number(ChNum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
}

void a_confdialog::SetRangeCB(int ChNum, int ChTypCB)
{
    QComboBox *mcb = this->findChild<QComboBox *>("inrange."+QString::number(ChNum));
    if (mcb == 0)
    {
        ACONFDBG;
        return;
    }
    QStringList sl = QStringList() << RT_MText;
    switch (ChTypCB)
    {
    case INTYPEMA:
        sl.insert(0, RT_mAText);
        break;
    case INTYPEV:
        sl.insert(0, RT_VText);
        break;
    default:
        break;
    }
    QStringListModel *slm = new QStringListModel;
    slm->setStringList(sl);
    mcb->setModel(slm);
}

void a_confdialog::SetOscDly(int dly)
{
    aconf->Bci_block.oscdly = dly;
}

void a_confdialog::SetChOsc(int isChecked)
{
    int ChNum = GetChNumFromObjectName(sender()->objectName());
    if (ChNum == -1)
        return;
    quint16 tmpint = 0x0001;
    tmpint = tmpint << ChNum;
    QComboBox *cb = this->findChild<QComboBox *>("oscsrccb."+QString::number(ChNum));
    QLabel *lbl = this->findChild<QLabel *>("oscsrcl."+QString::number(ChNum));
    if (isChecked == Qt::Checked)
    {
        aconf->Bci_block.discosc |= tmpint;
        if (cb != 0)
            cb->setVisible(true);
        if (lbl != 0)
            lbl->setVisible(true);
    }
    else
    {
        aconf->Bci_block.discosc &= ~tmpint;
        if (cb != 0)
            cb->setVisible(false);
        if (lbl != 0)
            lbl->setVisible(false);
    }
}

void a_confdialog::SetChOscSrc(int srctyp)
{
    int ChNum = GetChNumFromObjectName(sender()->objectName());
    if (ChNum == -1)
        return;
    quint8 tmpi = ChNum << 1;
    quint32 tmpint = srctyp << tmpi;
    quint32 tmpmask = ~(0x00000003 << tmpi);
    aconf->Bci_block.oscsrc &= tmpmask;
    aconf->Bci_block.oscsrc |= tmpint;
}

int a_confdialog::GetChNumFromObjectName(QString ObjectName)
{
    QStringList ObjectNameSl = ObjectName.split(".");
    int ChNum;
    bool ok;
    if (ObjectNameSl.size()>1)
        ChNum = ObjectNameSl.at(1).toInt(&ok);
    else
        return -1;
    if (!ok)
        return -1;
    return ChNum;
}

bool a_confdialog::CheckConf()
{
    bool NotGood = false;
    for (int i=0; i<16; i++)
    {
        if (aconf->Bci_block.in_min[i] > aconf->Bci_block.in_max[i])
            NotGood = true;
        if (aconf->Bci_block.in_vmin[i] > aconf->Bci_block.in_vmax[i])
            NotGood = true;
        if (aconf->Bci_block.setminmin[i] > aconf->Bci_block.setmin[i])
            NotGood = true;
        if (aconf->Bci_block.setminmin[i] < aconf->Bci_block.in_vmin[i])
            NotGood = true;
        if (aconf->Bci_block.setmin[i] < aconf->Bci_block.in_vmin[i])
            NotGood = true;
        if (aconf->Bci_block.setmax[i] < aconf->Bci_block.setmin[i])
            NotGood = true;
        if (aconf->Bci_block.setmax[i] > aconf->Bci_block.setmaxmax[i])
            NotGood = true;
        if (aconf->Bci_block.setmax[i] > aconf->Bci_block.in_vmax[i])
            NotGood = true;
        if (aconf->Bci_block.setmaxmax[i] > aconf->Bci_block.in_vmax[i])
            NotGood = true;
    }
    return NotGood;
}

void a_confdialog::SetIn()
{
    QStringList tmpsl = QStringList() << "inmin" << "inmax" << "invmin" << "invmax" << \
                                         "setminmin" << "setmin" << "setmax" << "setmaxmax";
    QDoubleSpinBox *spb = qobject_cast<QDoubleSpinBox *>(sender());
    QString ObjectName = spb->objectName().split(".").at(0);
    int idx = tmpsl.indexOf(ObjectName);
    if (idx == -1)
    {
        ACONFDBG;
        return;
    }
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == -1)
        return;
    QString tmps;
    switch(idx)
    {
    case 0: // inmin
        aconf->Bci_block.in_min[tmpi] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        break;
    case 1: // inmax
        aconf->Bci_block.in_max[tmpi] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        break;
    case 2: // invmin
        aconf->Bci_block.in_vmin[tmpi] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        break;
    case 3: // invmax
        aconf->Bci_block.in_vmax[tmpi] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        break;
    case 4: // setminmin
        aconf->Bci_block.setminmin[tmpi] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFRCLR)+";}";
        break;
    case 5: // setmin
        aconf->Bci_block.setmin[tmpi] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFYCLR)+";}";
        break;
    case 6: // setmax
        aconf->Bci_block.setmax[tmpi] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFYCLR)+";}";
        break;
    case 7: // setmaxmax
        aconf->Bci_block.setmaxmax[tmpi] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFRCLR)+";}";
        break;
    }
    if (CheckConf())
        tmps = "QDoubleSpinBox {color: "+QString(ERRCLR)+"; font: bold;}";
    spb->setStyleSheet(tmps);
}

void a_confdialog::Set104()
{
    QDoubleSpinBox *spb = qobject_cast<QDoubleSpinBox *>(sender());
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == -1)
        return;
    switch (tmpi)
    {
    case 0:
    {
        aconf->Bci_block.Abs_104=spb->value();
        break;
    }
    case 1:
    {
        aconf->Bci_block.Cycle_104=spb->value();
        break;
    }
    case 2:
    {
        aconf->Bci_block.T1_104=spb->value();
        break;
    }
    case 3:
    {
        aconf->Bci_block.T2_104=spb->value();
        break;
    }
    case 4:
    {
        aconf->Bci_block.T3_104=spb->value();
        break;
    }
    case 5:
    {
        aconf->Bci_block.k_104=spb->value();
        break;
    }
    case 6:
    {
        aconf->Bci_block.w_104=spb->value();
        break;
    }
    default:
        break;
    }
}

void a_confdialog::SetCType(int num)
{
    aconf->Bci_block.Ctype = num;
}

void a_confdialog::WriteConfDataToModule()
{
    if (CheckConf())
    {
        ACONFER("В конфигурации есть ошибки. Проверьте и исправьте");
        return;
    }
    cn->Send(CN_WF, &aconf->Bci_block, sizeof(aconf->Bci_block), 2, aconf->Config);
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
    {
        emit BsiIsNeedToBeAcquiredAndChecked();
        ACONFINFO("Переход прошёл успешно!");
    }
}

void a_confdialog::SetDefConf()
{
    memcpy(&(aconf->Bci_block), &(aconf->Bci_defblock), sizeof(aconf->Bci_block));
    FillConfData();
}

void a_confdialog::LoadConf()
{
    QByteArray ba;
    ba = pc.LoadFile("Config files (*.acf)");
    if (pc.RestoreDataMem(&(ba.data()[0]), ba.size(), aconf->Config))
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
    pc.StoreDataMem(&(ba->data()[0]), aconf->Config);
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

void a_confdialog::SetRangeWidgetSlot(QString RangeType)
{
    if (ChTypModelIsFilling) // выход, если слот вызван заполнением модели выпадающего списка
        return;
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
    QStringList RangeTypes = QStringList() << "Предуст. мА" << "Предуст. В" << "Произвольный";
    SetRangeWidget(ChNum, RangeTypes.indexOf(RangeType));
}

void a_confdialog::SetRangeWidget(int ChNum, int RangeType)
{
    QGroupBox *gb = this->findChild<QGroupBox *>("RangeGB");
    if (gb == 0)
    {
        ACONFDBG;
        return;
    }
    QGridLayout *lyout = static_cast<QGridLayout *>(gb->layout());
    if (lyout == 0)
    {
        ACONFDBG;
        return;
    }
    QLayoutItem *OldLayoutItem = lyout->itemAtPosition(ChNum+1, 2);
    if (OldLayoutItem != 0)
    {
        QWidget *wdgt = OldLayoutItem->widget();
        if (wdgt != 0)
        {
            wdgt->hide();
            delete wdgt;
        }
        QLayout *olyout = OldLayoutItem->layout();
        if (olyout != 0)
        {
            QLayoutItem *item;
            QWidget *widget;
            while ((item = olyout->takeAt(0)) != 0)
            {
                if ((widget = item->widget()) != 0)
                {
                    widget->hide();
                    delete widget;
                }
                else
                    delete item;
            }
        }
    }
    if (aconf->Bci_block.in_type[ChNum] == INTYPENA)
        return;
    QHBoxLayout *hlyout = new QHBoxLayout;
    switch (RangeType)
    {
    case RT_mA:
    {
        QComboBox *cb = new QComboBox;
        setStyleSheet("QComboBox {border: 1px solid gray; border-radius: 5px;}" \
                      "QComboBox::drop-down {background-color: rgba(100,100,100,255); width: 5px; border-style: none;}");
        cb->addItem("(4..20) мА");
        cb->addItem("(0..20) мА");
        cb->addItem("(0..5) мА");
        cb->setObjectName("rangemA."+QString::number(ChNum));
        connect(cb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetRangemA()));
        cb->setCurrentIndex(0);
        hlyout->addWidget(cb);
        lyout->addLayout(hlyout, ChNum+1, 2, 1, 1);
        SetRange(0, ChNum);
        break;
    }
    case RT_V:
    {
        QComboBox *cb = new QComboBox;
        setStyleSheet("QComboBox {border: 1px solid gray; border-radius: 5px;}" \
                      "QComboBox::drop-down {background-color: rgba(100,100,100,255); width: 5px; border-style: none;}");
        cb->addItem("(0..5) В");
        cb->addItem("(-5..5) В");
        cb->setObjectName("rangeV."+QString::number(ChNum));
        connect(cb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetRangeV()));
        cb->setCurrentIndex(0);
        hlyout->addWidget(cb);
        lyout->addLayout(hlyout, ChNum+1, 2, 1, 1);
        SetRange(0+RT_V05, ChNum);
        break;
    }
    case RT_M:
    {
        QDoubleSpinBox *dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("inmin."+QString::number(ChNum));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-20.0);
        dspbls->setMaximum(20.0);
        QString tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        dspbls->setValue(aconf->Bci_block.in_min[ChNum]);
        hlyout->addWidget(dspbls);
        dspbls = new QDoubleSpinBox(this);
        dspbls->setObjectName("inmax."+QString::number(ChNum));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-20.0);
        dspbls->setMaximum(20.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        dspbls->setValue(aconf->Bci_block.in_max[ChNum]);
        hlyout->addWidget(dspbls);
        lyout->addLayout(hlyout, ChNum+1, 2, 1, 1);
        break;
    }
    default:
        return;
        break;
    }
}
