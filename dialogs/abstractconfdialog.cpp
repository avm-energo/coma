#include "abstractconfdialog.h"
#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include "../widgets/s_tqcombobox.h"
#include "../widgets/s_tqspinbox.h"
#include "../widgets/wd_func.h"
#include "../canal.h"
// #include "../publicclass.h"

AbstractConfDialog::AbstractConfDialog(QObject *parent) : QDialog(parent)
{
    ConfigMain = new Config;
}

void AbstractConfDialog::Fill104()
{
    WDFunc::SetSPBData(this, "spb.1", ConfigMain->MainBlk.Abs_104);
    WDFunc::SetSPBData(this, "spb.2", ConfigMain->MainBlk.Cycle_104);
    WDFunc::SetSPBData(this, "spb.3", ConfigMain->MainBlk.T1_104);
    WDFunc::SetSPBData(this, "spb.4", ConfigMain->MainBlk.T2_104);
    WDFunc::SetSPBData(this, "spb.5", ConfigMain->MainBlk.T3_104);
    WDFunc::SetSPBData(this, "spb.6", ConfigMain->MainBlk.k_104);
    WDFunc::SetSPBData(this, "spb.7", ConfigMain->MainBlk.w_104);
    WDFunc::SetSPBData(this, "spb.8", ConfigMain->MainBlk.Ctype);
}

QWidget *AbstractConfDialog::Widget104()
{
    QWidget *w = new QWidget;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;
    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    QGridLayout *glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    QLabel *lbl = new QLabel("Адрес базовой станции:");
    glyout->addWidget(lbl,0,0,1,1,Qt::AlignRight);
    s_tqSpinBox *dspbls = WDFunc::NewSPB(w, "spb.1", 0, 65535, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);
    lbl = new QLabel("Интервал циклического опроса:");
    glyout->addWidget(lbl,1,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(w, "spb.2", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,1,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t1:");
    glyout->addWidget(lbl,2,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(w, "spb.3", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,2,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t2:");
    glyout->addWidget(lbl,3,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(w, "spb.4", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,3,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t3:");
    glyout->addWidget(lbl,4,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(w, "spb.5", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,4,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    glyout->addWidget(lbl,5,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(w, "spb.6", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,5,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    glyout->addWidget(lbl,6,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(w, "spb.7", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,6,2,1,1);
    lbl = new QLabel("Тип синхр. времени:");
    glyout->addWidget(lbl,7,0,1,1,Qt::AlignRight);
    QStringList cbl = QStringList() << "SNTP+PPS" << "SNTP";
    s_tqComboBox *cb = WDFunc::NewCB(w, "spb.8", cbl, ACONFGCLR);
    cb->setMinimumWidth(70);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    glyout->addWidget(cb, 7, 1, 1, 2);
    gb->setLayout(glyout);
    vlyout->addWidget(gb);
    w->setLayout(vlyout);
    return w;
}

void AbstractConfDialog::Set104(double dbl)
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
        ConfigMain->MainBlk.Abs_104=dbl;
        break;
    }
    case 2:
    {
        ConfigMain->MainBlk.Cycle_104=dbl;
        break;
    }
    case 3:
    {
        ConfigMain->MainBlk.T1_104=dbl;
        break;
    }
    case 4:
    {
        ConfigMain->MainBlk.T2_104=dbl;
        break;
    }
    case 5:
    {
        ConfigMain->MainBlk.T3_104=dbl;
        break;
    }
    case 6:
    {
        ConfigMain->MainBlk.k_104=dbl;
        break;
    }
    case 7:
    {
        ConfigMain->MainBlk.w_104=dbl;
        break;
    }
    default:
        break;
    }
}

void AbstractConfDialog::SetCType(int num)
{
    ConfigMain->MainBlk.Ctype = num;
}
