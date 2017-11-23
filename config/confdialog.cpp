#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include "confdialog.h"
#include "../widgets/s_tqcombobox.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/publicclass.h"

ConfDialog::ConfDialog(QVector<publicclass::DataRec> &S2Config, QWidget *parent) : AbstractConfDialog(parent)
{
    ConfigMain = new Config(S2Config); // добавляем к переданному S2Config общую часть
    SetupUI();
}

void ConfDialog::Fill()
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

void ConfDialog::FillBack()
{
    WDFunc::SPBData(this, "spb.1", ConfigMain->MainBlk.Abs_104);
    WDFunc::SPBData(this, "spb.2", ConfigMain->MainBlk.Cycle_104);
    WDFunc::SPBData(this, "spb.3", ConfigMain->MainBlk.T1_104);
    WDFunc::SPBData(this, "spb.4", ConfigMain->MainBlk.T2_104);
    WDFunc::SPBData(this, "spb.5", ConfigMain->MainBlk.T3_104);
    WDFunc::SPBData(this, "spb.6", ConfigMain->MainBlk.k_104);
    WDFunc::SPBData(this, "spb.7", ConfigMain->MainBlk.w_104);
    WDFunc::SPBData(this, "spb.8", ConfigMain->MainBlk.Ctype);
}

void ConfDialog::SetupUI()
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;
    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    QGridLayout *glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    QLabel *lbl = new QLabel("Адрес базовой станции:");
    glyout->addWidget(lbl,0,0,1,1,Qt::AlignRight);
    QDoubleSpinBox *dspbls = WDFunc::NewSPB(this, "spb.1", 0, 65535, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);
    lbl = new QLabel("Интервал циклического опроса:");
    glyout->addWidget(lbl,1,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(this, "spb.2", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,1,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t1:");
    glyout->addWidget(lbl,2,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(this, "spb.3", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,2,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t2:");
    glyout->addWidget(lbl,3,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(this, "spb.4", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,3,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t3:");
    glyout->addWidget(lbl,4,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(this, "spb.5", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,4,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    glyout->addWidget(lbl,5,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(this, "spb.6", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,5,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    glyout->addWidget(lbl,6,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(this, "spb.7", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,6,2,1,1);
    lbl = new QLabel("Тип синхр. времени:");
    glyout->addWidget(lbl,7,0,1,1,Qt::AlignRight);
    QStringList cbl = QStringList() << "SNTP+PPS" << "SNTP";
    s_tqComboBox *cb = WDFunc::NewCB(this, "spb.8", cbl, ACONFGCLR);
    cb->setMinimumWidth(70);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    glyout->addWidget(cb, 7, 1, 1, 2);
    gb->setLayout(glyout);
    vlyout->addWidget(gb);
    setLayout(vlyout);
}

void ConfDialog::SetDefConf()
{
    ConfigMain->MainBlk.Abs_104 = DEF_ABS_104;
    ConfigMain->MainBlk.Ctype = DEF_CTYPE;
    ConfigMain->MainBlk.Cycle_104 = DEF_CYCLE_104;
    ConfigMain->MainBlk.k_104 = DEF_K_104;
    ConfigMain->MainBlk.w_104 = DEF_W_104;
    ConfigMain->MainBlk.T1_104 = DEF_T1_104;
    ConfigMain->MainBlk.T2_104 = DEF_T2_104;
    ConfigMain->MainBlk.T3_104 = DEF_T3_104;
}

void ConfDialog::Set104(double dbl)
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

void ConfDialog::SetCType(int num)
{
    ConfigMain->MainBlk.Ctype = num;
}
