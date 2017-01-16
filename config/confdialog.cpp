#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QStringListModel>
#include <QSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include "../widgets/s_tqcombobox.h"
#include "../widgets/s_tqspinbox.h"
#include "../canal.h"
#include "confdialog.h"

confdialog::confdialog(Bci_Main *bci, QObject *parent) : QObject(parent)
{
    BciMain = bci;
    MainWin = new QWidget;
}

confdialog::~confdialog()
{
    delete MainWin;
}

void confdialog::Fill()
{
    s_tqSpinBox *dspbls;
    s_tqComboBox *cb;

    dspbls = MainWin->findChild<s_tqSpinBox *>("spb.1");
    if (dspbls != 0)
        dspbls->setValue(BciMain->Abs_104);
    dspbls = MainWin->findChild<s_tqSpinBox *>("spb.2");
    if (dspbls != 0)
        dspbls->setValue(BciMain->Cycle_104);
    dspbls = MainWin->findChild<s_tqSpinBox *>("spb.3");
    if (dspbls != 0)
        dspbls->setValue(BciMain->T1_104);
    dspbls = MainWin->findChild<s_tqSpinBox *>("spb.4");
    if (dspbls != 0)
        dspbls->setValue(BciMain->T2_104);
    dspbls = MainWin->findChild<s_tqSpinBox *>("spb.5");
    if (dspbls != 0)
        dspbls->setValue(BciMain->T3_104);
    dspbls = MainWin->findChild<s_tqSpinBox *>("spb.6");
    if (dspbls != 0)
        dspbls->setValue(BciMain->k_104);
    dspbls = MainWin->findChild<s_tqSpinBox *>("spb.7");
    if (dspbls != 0)
        dspbls->setValue(BciMain->w_104);
    cb = MainWin->findChild<s_tqComboBox *>("spb.8");
    if (cb != 0)
        cb->setCurrentIndex(BciMain->Ctype);
}

QWidget *confdialog::Widget104()
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    s_tqComboBox *cb = new s_tqComboBox;
    QGroupBox *gb = new QGroupBox;
    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    QGridLayout *gb3lyout = new QGridLayout;
    gb3lyout->setColumnStretch(2, 50);
    QLabel *lbl = new QLabel("Адрес базовой станции:");
    gb3lyout->addWidget(lbl,0,0,1,1,Qt::AlignRight);
    s_tqSpinBox *dspbls = new s_tqSpinBox;
    dspbls->setObjectName("spb.1");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(65535);
    QString tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    gb3lyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);
    lbl = new QLabel("Интервал циклического опроса:");
    gb3lyout->addWidget(lbl,1,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("spb.2");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    gb3lyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,1,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t1:");
    gb3lyout->addWidget(lbl,2,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("spb.3");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    gb3lyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,2,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t2:");
    gb3lyout->addWidget(lbl,3,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("spb.4");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    gb3lyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,3,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t3:");
    gb3lyout->addWidget(lbl,4,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("spb.5");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    gb3lyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,4,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    gb3lyout->addWidget(lbl,5,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("spb.6");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    gb3lyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,5,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    gb3lyout->addWidget(lbl,6,0,1,1,Qt::AlignRight);
    dspbls = new s_tqSpinBox;
    dspbls->setObjectName("spb.7");
    dspbls->setSingleStep(1);
    dspbls->setDecimals(0);
    dspbls->setMinimum(0);
    dspbls->setMaximum(255);
    tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
    dspbls->setStyleSheet(tmps);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    gb3lyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    gb3lyout->addWidget(lbl,6,2,1,1);
    lbl = new QLabel("Тип синхр. времени:");
    gb3lyout->addWidget(lbl,7,0,1,1,Qt::AlignRight);
    cb->setObjectName("spb.8");
    QStringList cbl = QStringList() << "SNTP+PPS" << "SNTP";
    QStringListModel *cblm = new QStringListModel;
    cblm->setStringList(cbl);
    cb->setModel(cblm);
    cb->setMinimumWidth(70);
    tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
    cb->setStyleSheet(tmps);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    gb3lyout->addWidget(cb, 7, 1, 1, 2);
    gb->setLayout(gb3lyout);
    vlyout->addWidget(gb);
    MainWin->setLayout(vlyout);
    return MainWin;
}

void confdialog::Set104(double dbl)
{
    QStringList sl = sender()->objectName().split(".");
    if (sl.size() < 1)
    {
        DBGMSG;
        return;
    }
    bool ok;
    int wnum = sl.at(1).toInt(&ok);
    if (!ok)
    {
        DBGMSG;
        return;
    }
    switch (wnum)
    {
    case 1:
    {
        BciMain->Abs_104=dbl;
        break;
    }
    case 2:
    {
        BciMain->Cycle_104=dbl;
        break;
    }
    case 3:
    {
        BciMain->T1_104=dbl;
        break;
    }
    case 4:
    {
        BciMain->T2_104=dbl;
        break;
    }
    case 5:
    {
        BciMain->T3_104=dbl;
        break;
    }
    case 6:
    {
        BciMain->k_104=dbl;
        break;
    }
    case 7:
    {
        BciMain->w_104=dbl;
        break;
    }
    default:
        break;
    }
}

void confdialog::SetCType(int num)
{
    BciMain->Ctype = num;
}
