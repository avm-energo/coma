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

AbstractConfDialog2x::AbstractConfDialog2x(QVector<publicclass::DataRec> &S2Config, bool BaseBoard, QWidget *parent) :
    AbstractConfDialog(S2Config, parent)
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
    QSpinBox *spb = WDFunc::NewSPB(this, "oscdlyspb", 0, 10000, 1, 0, ACONFGCLR);
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
    for (i = 1; i <= Params.NumCh; i++)
    {
        lbl = new QLabel(QString::number(i));
        glyout->addWidget(lbl,i,0,1,1);
        QComboBox *mcb = WDFunc::NewCB(this, "inrange."+QString::number(i), Params.RangeTypes);
        connect(mcb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetRangeWidgetSlot(int)));
        glyout->addWidget(mcb, i,1,1,1);
        dspbls = WDFunc::NewSPB(this, "invmin."+QString::number(i), -100000.0, 100000.0, 0.01, 2, ACONFGCLR);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        glyout->addWidget(dspbls,i,3,1,1);
        dspbls = WDFunc::NewSPB(this, "invmax."+QString::number(i), -100000.0, 100000.0, 0.01, 2, ACONFGCLR);;
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
    for (i = 0; i < AIN21_NUMCH; i++)
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

void AbstractConfDialog2x::SetMinMax(int i)
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

void AbstractConfDialog2x::DisableChannel(int chnum, bool disable)
{
    bool Enabled = !disable;
    WDFunc::SetEnabled(this, "chb."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "choscdi1."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "choscdi2."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "choscthr."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "chosccso0."+QString::number(chnum), Enabled);
    cb = this->findChild<QComboBox *>("inrange."+QString::number(chnum));
    if (cb != 0)
        cb->setVisible(!Disable);
    cb = this->findChild<QComboBox *>("rangemA."+QString::number(chnum));
    if (cb != 0)
        cb->setVisible(!Disable);
    cb = this->findChild<QComboBox *>("rangeV."+QString::number(chnum));
    if (cb != 0)
        cb->setVisible(!Disable);
    QDoubleSpinBox *dspbls = this->findChild<QDoubleSpinBox *>("inmin."+QString::number(chnum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("inmax."+QString::number(chnum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("invmin."+QString::number(chnum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("invmax."+QString::number(chnum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("setminmin."+QString::number(chnum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("setmin."+QString::number(chnum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("setmax."+QString::number(chnum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
    dspbls = this->findChild<QDoubleSpinBox *>("setmaxmax."+QString::number(chnum));
    if (dspbls != 0)
        dspbls->setVisible(!Disable);
}

void AbstractConfDialog2x::SetRangeCB(int ChNum, int ChTypCB)
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

void AbstractConfDialog2x::SetOscDly(int dly)
{
    aconf->Bci_block.oscdly = dly;
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

void AbstractConfDialog2x::SetChOscSrc(int srctyp)
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

void AbstractConfDialog2x::SetIn()
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

void AbstractConfDialog2x::SetRangeWidgetSlot(int RangeType)
{
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

void AbstractConfDialog2x::SetRangeWidget(int ChNum, int RangeType)
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
