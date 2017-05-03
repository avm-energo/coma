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
#include "confdialog_21.h"
#include "confdialog.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"
#include "../canal.h"

confdialog_21::confdialog_21(QWidget *parent) :
    QDialog(parent)
{
    ChTypModelIsFilling = false;
    aconf = new config_21;
    NoProperConf = false;

    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    if ((pc.ModuleBsi.Hth & HTH_CONFIG) || (pc.Emul)) // если в модуле нет конфигурации, заполнить поля по умолчанию
        SetDefConf();
    else // иначе заполнить значениями из модуля
        GetBci();
}

void confdialog_21::SetupUI()
{
    int i;
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    QVBoxLayout *lyout = new QVBoxLayout;

    QGroupBox *gb = new QGroupBox("Типы каналов");
    QVBoxLayout *gblyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QStringList ChTypSl = QStringList() << "Не исп." << "мА" << "В";
    QStringListModel *ChTypSlM = new QStringListModel;
    ChTypSlM->setStringList(ChTypSl);
    for (i = 1; i <= AIN_NUMCH; ++i)
    {
        QLabel *ChTypL = new QLabel(QString::number(i)+":");
        QComboBox *ChTypCB = new QComboBox;
        ChTypCB->setModel(ChTypSlM);
        ChTypCB->setObjectName("chtypcb."+QString::number(i));
        tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
        ChTypCB->setStyleSheet(tmps);
        connect(ChTypCB,SIGNAL(currentIndexChanged(int)),this,SLOT(SetChTypData()));
        hlyout->addWidget(ChTypL);
        hlyout->addWidget(ChTypCB, 1);
        if ((i % 4) == 0)
        {
            gblyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    gb->setLayout(gblyout);
    lyout->addWidget(gb);
    gb = new QGroupBox("Осциллограммы");
    gblyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QLabel *lbl = new QLabel("Источники записи осциллограмм:");
    gblyout->addWidget(lbl);
    lbl = new QLabel("#");
    glyout->addWidget(lbl,0,0,1,1,Qt::AlignCenter);
    lbl = new QLabel("Вход DI2");
    glyout->addWidget(lbl,0,1,1,1,Qt::AlignCenter);
    lbl = new QLabel("Вход DI1");
    glyout->addWidget(lbl,0,2,1,1,Qt::AlignCenter);
    lbl = new QLabel("Превыш. oscthr");
    glyout->addWidget(lbl,0,3,1,1,Qt::AlignCenter);
    lbl = new QLabel("Команда Cso0");
    glyout->addWidget(lbl,0,4,1,1,Qt::AlignCenter);
    lbl = new QLabel("#");
    glyout->addWidget(lbl,0,5,1,1,Qt::AlignCenter);
    lbl = new QLabel("Вход DI2");
    glyout->addWidget(lbl,0,6,1,1,Qt::AlignCenter);
    lbl = new QLabel("Вход DI1");
    glyout->addWidget(lbl,0,7,1,1,Qt::AlignCenter);
    lbl = new QLabel("Превыш. oscthr");
    glyout->addWidget(lbl,0,8,1,1,Qt::AlignCenter);
    lbl = new QLabel("Команда Cso0");
    glyout->addWidget(lbl,0,9,1,1,Qt::AlignCenter);
    int row = 1;
    int column = 0;
    for (i=1; i<=AIN_NUMCH; ++i)
    {
        lbl=new QLabel(QString::number(i));
        glyout->addWidget(lbl,row,++column,1,1,Qt::AlignCenter);
        s_tqCheckBox *chb = WDFunc::NewCB("choscdi2."+QString::number(i), \
                                          "", \
                                          ACONFCLR);
        connect(chb,SIGNAL(stateChanged(int)),this, SLOT(SetChOsc(int)));
        glyout->addWidget(chb,row,++column,1,1,Qt::AlignCenter);
        chb = WDFunc::NewCB("choscdi1."+QString::number(i), \
                            "", \
                            ACONFCLR);
        connect(chb,SIGNAL(stateChanged(int)),this, SLOT(SetChOsc(int)));
        glyout->addWidget(chb,row,++column,1,1,Qt::AlignCenter);
        chb = WDFunc::NewCB("choscthr."+QString::number(i), \
                            "", \
                            ACONFCLR);
        connect(chb,SIGNAL(stateChanged(int)),this, SLOT(SetChOsc(int)));
        glyout->addWidget(chb,row,++column,1,1,Qt::AlignCenter);
        chb = WDFunc::NewCB("chosccso0."+QString::number(i), \
                            "", \
                            ACONFCLR);
        connect(chb,SIGNAL(stateChanged(int)),this, SLOT(SetChOsc(int)));
        glyout->addWidget(chb,row,++column,1,1,Qt::AlignCenter);
        if ((i%2) == 0)
        {
            ++row;
            column = 0;
        }
    }
    gblyout->addLayout(glyout);
    ChTypSl = QStringList() << "Ком. Ц" << "Ц+U>" << "Ц+DI" << "Любой";
    ChTypSlM = new QStringListModel;
    ChTypSlM->setStringList(ChTypSl);
    lbl = new QLabel("События-инициаторы запуска осциллограмм:");
    gblyout->addWidget(lbl);
    hlyout = new QHBoxLayout;
    for (i = 0; i < AIN_NUMCH; i++)
    {
        QLabel *ChTypL = new QLabel(QString::number(i)+":");
        ChTypL->setObjectName("oscsrcl."+QString::number(i));
        QComboBox *ChTypCB = new QComboBox;
        ChTypCB->setObjectName("oscsrccb."+QString::number(i));
        ChTypCB->setModel(ChTypSlM);
        tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
        ChTypCB->setStyleSheet(tmps);
        connect(ChTypCB,SIGNAL(currentIndexChanged(int)),this,SLOT(SetChOscSrc(int)));
        hlyout->addWidget(ChTypL);
        hlyout->addWidget(ChTypCB, 1);
        if ((i>1)&&!((i+1)%4))
        {
            gblyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    gblyout->addLayout(hlyout);
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
    lyout->addWidget(gb);
    QWidget *cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp->setStyleSheet(tmps);
    cp->setLayout(lyout);
    ConfTW->addTab(cp,"Общие");

    lyout = new QVBoxLayout;
    gb = new QGroupBox("Диапазоны сигналов");
    gb->setObjectName("RangeGB");
    glyout = new QGridLayout;
    glyout->setColumnStretch(0,0);
    glyout->setColumnStretch(1,10);
    glyout->setColumnStretch(2,5);
    glyout->setColumnStretch(3,5);
    glyout->setColumnStretch(4,5);
    glyout->setColumnStretch(5,5);
    QDoubleSpinBox *dspbls;
    lbl = new QLabel("№ канала");
    glyout->addWidget(lbl,0,0,1,1);
    lbl = new QLabel("Тип диапазона");
    glyout->addWidget(lbl,0,1,1,1);
    lbl = new QLabel("Диапазон");
    glyout->addWidget(lbl,0,2,1,2);
    lbl = new QLabel("Мин. инж.");
    glyout->addWidget(lbl,0,3,1,1);
    lbl = new QLabel("Макс. инж.");
    glyout->addWidget(lbl,0,4,1,1);
    QStringList sl = QStringList() << "Предуст. мА" << "Произвольный";
    QStringListModel *slm = new QStringListModel;
    slm->setStringList(sl);
    for (i = 0; i < 16; i++)
    {
        QLabel *ChTypL = new QLabel(QString::number(i));
        glyout->addWidget(ChTypL,i+1,0,1,1);
        QComboBox *mcb = new QComboBox;
        mcb->setObjectName("inrange."+QString::number(i));
        mcb->setModel(slm);
        glyout->addWidget(mcb, i+1,1,1,1);
        connect(mcb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetRangeWidgetSlot(QString)));
        dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("invmin."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i+1,3,1,1);
        dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("invmax."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i+1,4,1,1);
    }
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp->setStyleSheet(tmps);
    cp->setLayout(lyout);
    ConfTW->addTab(cp,"Диапазоны");

    lyout = new QVBoxLayout;
    gb = new QGroupBox("Уставки");
    glyout = new QGridLayout;
    lbl = new QLabel("№ канала");
    glyout->addWidget(lbl,0,0,1,1,Qt::AlignCenter);
    lbl = new QLabel("Мин. авар.");
    glyout->addWidget(lbl,0,1,1,1,Qt::AlignCenter);
    lbl = new QLabel("Мин. пред.");
    glyout->addWidget(lbl,0,2,1,1,Qt::AlignCenter);
    lbl = new QLabel("Макс. пред.");
    glyout->addWidget(lbl,0,3,1,1,Qt::AlignCenter);
    lbl = new QLabel("Макс. авар.");
    glyout->addWidget(lbl,0,4,1,1,Qt::AlignCenter);
    for (i = 0; i < 16; i++)
    {
        QLabel *ChTypL = new QLabel(QString::number(i));
        glyout->addWidget(ChTypL,i+1,0,1,1,Qt::AlignRight);
        dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("setminmin."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFRCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i+1,1,1,1,Qt::AlignCenter);
        dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("setmin."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFYCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i+1,2,1,1,Qt::AlignCenter);
        dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("setmax."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFYCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i+1,3,1,1,Qt::AlignCenter);
        dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("setmaxmax."+QString::number(i));
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-100000.0);
        dspbls->setMaximum(100000.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFRCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i+1,4,1,1,Qt::AlignCenter);
    }
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp->setStyleSheet(tmps);
    cp->setLayout(lyout);
    ConfTW->addTab(cp,"Уставки");

    gb = new QGroupBox("Настройки протокола МЭК-60870-5-104");
    confdialog *dlg = new confdialog(&aconf->Bci_block.mainblk);
    QWidget *w = dlg->Widget104();
    connect(this,SIGNAL(Fill104Conf()),dlg,SLOT(Fill()));
    gblyout = new QVBoxLayout;
    gblyout->addWidget(w);
    gb->setLayout(gblyout);
    lyout->addWidget(gb);
    cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp->setStyleSheet(tmps);
    cp->setLayout(lyout);
    ConfTW->addTab(cp,"104");

    lyout = new QVBoxLayout;
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

void confdialog_21::SetRangemA()
{
    QComboBox *cb = qobject_cast<QComboBox *>(sender());
    if (cb == 0)
        return;
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == -1)
        return;
    SetRange(cb->currentIndex(), tmpi);
}

void confdialog_21::SetRangeV()
{
    QComboBox *cb = qobject_cast<QComboBox *>(sender());
    if (cb == 0)
        return;
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == -1)
        return;
    SetRange(cb->currentIndex()+RT_V05, tmpi); // перемещаем диапазон комбобокса в область V
}

void confdialog_21::SetRange(int Range, int ChNum)
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

void confdialog_21::GetBci()
{
    cn->Send(CN_GF,Canal::BT_NONE,NULL,0,1,aconf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == NOERROR)
        FillConfData();
    else
        MessageBox2::error(this, "ошибка", "Ошибка чтения конфигурации из модуля");
}

void confdialog_21::FillConfData()
{
    int i;
    QComboBox *cb;
    QCheckBox *chb;

    WDFunc::SetSPBData(this, "oscdlyspb", aconf->Bci_block.oscdly);
    for (i = 0; i < 16; i++)
    {
        WDFunc::SetCBIndex(this, "chtypcb."+QString::number(i), aconf->Bci_block.in_type[i]);
        WDFunc::SetChBData(this, "chb."+QString::number(i), aconf->Bci_block.osc[i]);
        cb = this->findChild<QComboBox *>("oscsrccb."+QString::number(i));
        if (cb == 0)
        {
            DBGMSG;
            return;
        }
        QLabel *lbl = this->findChild<QLabel *>("oscsrcl."+QString::number(i));
        if (lbl == 0)
        {
            DBGMSG;
            return;
        }
//        quint8 tmpi = i << 1;
        cb->setCurrentIndex(aconf->Bci_block.osc[i]);
        SetMinMax(i);
        WDFunc::SetSPBData(this, "invmin."+QString::number(i), aconf->Bci_block.in_vmin[i]);
        WDFunc::SetSPBData(this, "invmax."+QString::number(i), aconf->Bci_block.in_vmax[i]);
        WDFunc::SetSPBData(this, "setminmin."+QString::number(i), aconf->Bci_block.setminmin[i]);
        WDFunc::SetSPBData(this, "setmin."+QString::number(i), aconf->Bci_block.setmin[i]);
        WDFunc::SetSPBData(this, "setmax."+QString::number(i), aconf->Bci_block.setmax[i]);
        WDFunc::SetSPBData(this, "setmaxmax."+QString::number(i), aconf->Bci_block.setmaxmax[i]);
        DisableChannel(i, (aconf->Bci_block.in_type[i] == INTYPENA));
    }
}

void confdialog_21::SetMinMax(int i)
{
    QComboBox *cb = this->findChild<QComboBox *>("inrange."+QString::number(i));
    if (cb == 0)
    {
        DBGMSG;
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

void confdialog_21::SetChTypData()
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

void confdialog_21::DisableChannel(int ChNum, bool Disable)
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

void confdialog_21::SetRangeCB(int ChNum, int ChTypCB)
{
    QComboBox *mcb = this->findChild<QComboBox *>("inrange."+QString::number(ChNum));
    if (mcb == 0)
    {
        DBGMSG;
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

void confdialog_21::SetOscDly(int dly)
{
    aconf->Bci_block.oscdly = dly;
}

void confdialog_21::SetChOsc(int isChecked)
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
/* !!!        aconf->Bci_block.discosc |= tmpint;
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
            lbl->setVisible(false); */
    }
}

void confdialog_21::SetChOscSrc(int srctyp)
{
    int ChNum = GetChNumFromObjectName(sender()->objectName());
    if (ChNum == -1)
        return;
    quint8 tmpi = ChNum << 1;
    quint32 tmpint = srctyp << tmpi;
    quint32 tmpmask = ~(0x00000003 << tmpi);
/* !!!    aconf->Bci_block.oscsrc &= tmpmask;
    aconf->Bci_block.oscsrc |= tmpint; */
}

int confdialog_21::GetChNumFromObjectName(QString ObjectName)
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

bool confdialog_21::CheckConf()
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

void confdialog_21::SetIn()
{
    QStringList tmpsl = QStringList() << "inmin" << "inmax" << "invmin" << "invmax" << \
                                         "setminmin" << "setmin" << "setmax" << "setmaxmax";
    QDoubleSpinBox *spb = qobject_cast<QDoubleSpinBox *>(sender());
    QString ObjectName = spb->objectName().split(".").at(0);
    int idx = tmpsl.indexOf(ObjectName);
    if (idx == -1)
    {
        DBGMSG;
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

void confdialog_21::WriteConfDataToModule()
{
    if (CheckConf())
    {
        ERMSG("В конфигурации есть ошибки. Проверьте и исправьте");
        return;
    }
    cn->Send(CN_WF, Canal::BT_NONE, &aconf->Bci_block, sizeof(aconf->Bci_block), 2, aconf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == NOERROR)
        MessageBox2::information(this, "Внимание", "Операция проведена успешно!");
}

void confdialog_21::SetNewConf()
{
/*    cn->Send(CN_Cnc);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == NOERROR)
    {
        emit BsiIsNeedToBeAcquiredAndChecked();
        MessageBox2::information(this, "Внимание", "Переход прошёл успешно!");
    } */
}

void confdialog_21::SetDefConf()
{
    memcpy(&(aconf->Bci_block), &(aconf->Bci_defblock), sizeof(aconf->Bci_block));
    FillConfData();
//    emit Set104Conf(aconf->Bci_block.mainblk);
    emit Fill104Conf();
    MessageBox2::information(this, "Успешно", "Задана конфигурация по умолчанию");
}

void confdialog_21::LoadConf()
{
    QByteArray ba;
    ba = pc.LoadFile("Config files (*.acf)");
    if (pc.RestoreDataMem(&(ba.data()[0]), ba.size(), aconf->Config))
    {
        WARNMSG("");
        return;
    }
    FillConfData();
    MessageBox2::information(this, "Внимание", "Загрузка прошла успешно!");
}

void confdialog_21::SaveConf()
{
    QByteArray *ba = new QByteArray;
    ba->resize(MAXBYTEARRAY);
    pc.StoreDataMem(&(ba->data()[0]), aconf->Config, 0x0001); // 0x101 - номер файла конфигурации
    quint32 BaLength = static_cast<quint8>(ba->data()[4]);
    BaLength += static_cast<quint8>(ba->data()[5])*256;
    BaLength += static_cast<quint8>(ba->data()[6])*65536;
    BaLength += static_cast<quint8>(ba->data()[7])*16777216;
    BaLength += sizeof(publicclass::FileHeader); // FileHeader
    int res = pc.SaveFile("Config files (*.acf)", &(ba->data()[0]), BaLength);
    switch (res)
    {
    case 0:
        MessageBox2::information(this, "Внимание", "Записано успешно!");
        break;
    case 1:
        ERMSG("Ошибка при записи файла!");
        break;
    case 2:
        ERMSG("Пустое имя файла!");
        break;
    case 3:
        ERMSG("Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void confdialog_21::SetRangeWidgetSlot(QString RangeType)
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
        WARNMSG("");
        return;
    }
    QStringList RangeTypes = QStringList() << "Предуст. мА" << "Предуст. В" << "Произвольный";
    SetRangeWidget(ChNum, RangeTypes.indexOf(RangeType));
}

void confdialog_21::SetRangeWidget(int ChNum, int RangeType)
{
    QGroupBox *gb = this->findChild<QGroupBox *>("RangeGB");
    if (gb == 0)
    {
        DBGMSG;
        return;
    }
    QGridLayout *lyout = static_cast<QGridLayout *>(gb->layout());
    if (lyout == 0)
    {
        DBGMSG;
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
