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
#include "abstractconfdialog2x.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"
#include "../canal.h"

AbstractConfDialog2x::AbstractConfDialog2x(bool BaseBoard, QWidget *parent) :
    AbstractConfDialog(parent)
{
    isBaseBoard = BaseBoard;
    NoProperConf = false;
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void AbstractConfDialog2x::SetupUI()
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
    for (i = 1; i <= Params.NumCh; ++i)
    {
        QLabel *ChTypL = new QLabel(QString::number(i)+":");
        s_tqComboBox *cb = WDFunc::NewCB(this, "chtypcb."+QString::number(i), Params.InTypes, ACONFGCLR);
        connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetChTypData()));
        hlyout->addWidget(ChTypL);
        hlyout->addWidget(cb, 1);
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
    for (i=1; i<=Params.NumCh; ++i)
    {
        lbl=new QLabel(QString::number(i));
        glyout->addWidget(lbl,row,++column,1,1,Qt::AlignCenter);
        s_tqCheckBox *chb = WDFunc::NewChB(this, "choscdi2."+QString::number(i), "", ACONFCLR);
        connect(chb,SIGNAL(stateChanged(int)),this, SLOT(SetChOsc(int)));
        glyout->addWidget(chb,row,++column,1,1,Qt::AlignCenter);
        chb = WDFunc::NewChB(this, "choscdi1."+QString::number(i), "", ACONFCLR);
        connect(chb,SIGNAL(stateChanged(int)),this, SLOT(SetChOsc(int)));
        glyout->addWidget(chb,row,++column,1,1,Qt::AlignCenter);
        chb = WDFunc::NewChB(this, "choscthr."+QString::number(i), "", ACONFCLR);
        connect(chb,SIGNAL(stateChanged(int)),this, SLOT(SetChOsc(int)));
        glyout->addWidget(chb,row,++column,1,1,Qt::AlignCenter);
        chb = WDFunc::NewChB(this, "chosccso0."+QString::number(i), "", ACONFCLR);
        connect(chb,SIGNAL(stateChanged(int)),this, SLOT(SetChOsc(int)));
        glyout->addWidget(chb,row,++column,1,1,Qt::AlignCenter);
        if ((i%2) == 0)
        {
            ++row;
            column = 0;
        }
    }
    gblyout->addLayout(glyout);
    lbl = new QLabel("Задержка в мс начала фиксации максимумов:");
    gblyout->addWidget(lbl);
    s_tqSpinBox *spb = WDFunc::NewSPB(this, "oscdlyspb", 0, 10000, 1, 0, ACONFGCLR);
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
    gb = new QGroupBox("Входные сигналы");
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
    lbl = new QLabel("Диапазон");
    glyout->addWidget(lbl,0,1,1,1);
    lbl = new QLabel("Границы сигнала");
    glyout->addWidget(lbl,0,2,1,2);
    lbl = new QLabel("Мин. инж.");
    glyout->addWidget(lbl,0,3,1,1);
    lbl = new QLabel("Макс. инж.");
    glyout->addWidget(lbl,0,4,1,1);
    for (i = 1; i <= Params.NumCh; ++i)
    {
        lbl = new QLabel(QString::number(i));
        glyout->addWidget(lbl,i,0,1,1);
        QComboBox *mcb = WDFunc::NewCB(this, "inrange."+QString::number(i), Params.RangeTypes);
        connect(mcb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetRange(int)));
        glyout->addWidget(mcb, i,1,1,1);
        dspbls = WDFunc::NewSPB(this, "0."+QString::number(i), -20.0, 20.0, 0.01, 2, ACONFGCLR); // inmin
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        hlyout->addWidget(dspbls);
        dspbls = WDFunc::NewSPB(this, "1."+QString::number(i), -20.0, 20.0, 0.01, 2, ACONFGCLR); // inmax
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        hlyout->addWidget(dspbls);
        lbl = new QLabel("мА");
        lbl->setObjectName("units");
        hlyout->addWidget(lbl);
        glyout->addLayout(hlyout, i, 2, 1, 1);
        dspbls = WDFunc::NewSPB(this, "2."+QString::number(i), -100000.0, 100000.0, 0.01, 2, ACONFGCLR); // invmin
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i,3,1,1);
        dspbls = WDFunc::NewSPB(this, "3."+QString::number(i), -100000.0, 100000.0, 0.01, 2, ACONFGCLR); // invmax
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i,4,1,1);
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
    for (i = 1; i <= Params.NumCh; ++i)
    {
        QLabel *ChTypL = new QLabel(QString::number(i));
        glyout->addWidget(ChTypL,i,0,1,1,Qt::AlignRight);
        dspbls = WDFunc::NewSPB(this, "4."+QString::number(i), -100000.0, 100000.0, 0.01, 2, ACONFRCLR); // setminmin
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i,1,1,1,Qt::AlignCenter);
        dspbls = WDFunc::NewSPB(this, "5."+QString::number(i), -100000.0, 100000.0, 0.01, 2, ACONFYCLR); // setmin
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i,2,1,1,Qt::AlignCenter);
        dspbls = WDFunc::NewSPB(this, "6."+QString::number(i), -100000.0, 100000.0, 0.01, 2, ACONFYCLR); // setmax
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i,3,1,1,Qt::AlignCenter);
        dspbls = WDFunc::NewSPB(this, "7."+QString::number(i), -100000.0, 100000.0, 0.01, 2, ACONFRCLR); // setmaxmax
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i,4,1,1,Qt::AlignCenter);
    }
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp->setStyleSheet(tmps);
    cp->setLayout(lyout);
    ConfTW->addTab(cp,"Уставки");

    if (isBaseBoard)
    {
        gb = new QGroupBox("Настройки протокола МЭК-60870-5-104");
        QWidget *w = Widget104();
        gblyout = new QVBoxLayout;
        gblyout->addWidget(w);
        gb->setLayout(gblyout);
        lyout->addWidget(gb);
        cp = new QWidget;
        tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
        cp->setStyleSheet(tmps);
        cp->setLayout(lyout);
        ConfTW->addTab(cp,"104");
    }

    lyout = new QVBoxLayout;
    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void AbstractConfDialog2x::DisableChannel(int chnum, bool disable)
{
    bool Enabled = !disable;
    WDFunc::SetEnabled(this, "chb."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "choscdi1."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "choscdi2."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "choscthr."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "chosccso0."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "inrange."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "0."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "1."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "2."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "3."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "4."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "5."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "6."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "7."+QString::number(chnum), Enabled);
}

void AbstractConfDialog2x::SetChOsc(int isChecked)
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
/* !!!        C21->Bci_block.inblk.discosc |= tmpint;
        if (cb != 0)
            cb->setVisible(true);
        if (lbl != 0)
            lbl->setVisible(true);
    }
    else
    {
        C21->Bci_block.inblk.discosc &= ~tmpint;
        if (cb != 0)
            cb->setVisible(false);
        if (lbl != 0)
            lbl->setVisible(false); */
    }
}

void AbstractConfDialog2x::SetChOscSrc(int srctyp)
{
    int ChNum = GetChNumFromObjectName(sender()->objectName());
    if (ChNum == -1)
        return;
    quint8 tmpi = ChNum << 1;
    quint32 tmpint = srctyp << tmpi;
    quint32 tmpmask = ~(0x00000003 << tmpi);
/* !!!    C21->Bci_block.inblk.oscsrc &= tmpmask;
    C21->Bci_block.inblk.oscsrc |= tmpint; */
}

int AbstractConfDialog2x::GetChNumFromObjectName(QString ObjectName)
{
    QStringList ObjectNameSl = ObjectName.split(".");
    int ChNum;
    bool ok;
    if (ObjectNameSl.size()>1)
        ChNum = ObjectNameSl.at(1).toInt(&ok);
    else
        return GENERALERROR;
    if (!ok)
        return GENERALERROR;
    return ChNum;
}

void AbstractConfDialog2x::PrereadConf()
{
    if ((pc.ModuleBsi.Hth & HTH_CONFIG) || (pc.Emul)) // если в модуле нет конфигурации, заполнить поля по умолчанию
        emit LoadDefConf();
    else // иначе заполнить значениями из модуля
        ReadConf();
}
