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
#include "confdialog21.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

ConfDialog21::ConfDialog21(QVector<publicclass::DataRec> &S2Config, bool BaseBoard, QWidget *parent) :
    AbstractConfDialog(parent)
{
    RangeInMins = {4.0, 0.0, 0.0, -5.0, 0.0};
    RangeInMaxs = {20.0, 20.0, 5.0, 5.0, 5.0};
    this->S2Config = &S2Config;
    C21 = new Config21(S2Config, BaseBoard);
    Params.InTypes = QStringList() << "Не исп." << "мА" << "В";
    Params.RangeTypes = QStringList() << "(4..20) мА" << "(0..20) мА" << "(0..5) мА" << \
                                         "(-5..5) В" << "(0..5) В" << "Произвольный мА" << "Произвольный В";
    Params.NumCh = AIN21_NUMCH;
    SetupUI();
    PrereadConf();
}

void ConfDialog21::SetupUI()
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
    for (i = 0; i < Params.NumCh; ++i)
    {
        QLabel *ChTypL = new QLabel(QString::number(i+1)+":");
        EComboBox *cb = WDFunc::NewCB(this, "chtypcb."+QString::number(i), Params.InTypes, ACONFWCLR);
        connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetChTypData(int)));
        hlyout->addWidget(ChTypL);
        hlyout->addWidget(cb, 1);
        if (((i+1) % 4) == 0)
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
    for (i=0; i<Params.NumCh; ++i)
    {
        lbl=new QLabel(QString::number(i+1));
        glyout->addWidget(lbl,row,column++,1,1,Qt::AlignCenter);
        QCheckBox *chb = WDFunc::NewChB(this, "choscdi2."+QString::number(i), "", ACONFWCLR);
        glyout->addWidget(chb,row,column++,1,1,Qt::AlignCenter);
        chb = WDFunc::NewChB(this, "choscdi1."+QString::number(i), "", ACONFWCLRO);
        glyout->addWidget(chb,row,column++,1,1,Qt::AlignCenter);
        chb = WDFunc::NewChB(this, "choscthr."+QString::number(i), "", ACONFWCLR);
        glyout->addWidget(chb,row,column++,1,1,Qt::AlignCenter);
        chb = WDFunc::NewChB(this, "chosccso0."+QString::number(i), "", ACONFWCLRO);
        glyout->addWidget(chb,row,column++,1,1,Qt::AlignCenter);
        if ((i%2) != 0)
        {
            ++row;
            column = 0;
        }
    }
    gblyout->addLayout(glyout);
    hlyout = new QHBoxLayout;
    lbl = new QLabel();
    hlyout->addWidget(WDFunc::NewLBLT(this, "Задержка в мс начала фиксации максимумов:"));
    hlyout->addWidget(WDFunc::NewSPB(this, "oscdly", 0, 10000, 0, ACONFWCLR));
    hlyout->addWidget(WDFunc::NewLBLT(this, "Гистерезис осциллограмм:", "", "", "Доля от диапазона, при превышении которой начинается запись осциллограмм"));
    hlyout->addWidget(WDFunc::NewSPB(this, "oscthr", 0, 1, 3, ACONFWCLR));
    gblyout->addLayout(hlyout);
    gb->setLayout(gblyout);
    lyout->addWidget(gb);

    gblyout = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    hlyout->addStretch(10);
    hlyout->addWidget(WDFunc::NewLBLT(this, "Степень фильтрации:", "", "", "Степень фильтрации (0 - отсутствует)"));
    hlyout->addWidget(WDFunc::NewSPB(this, "filtern", 0, 10000, 0, ACONFWCLR));
    hlyout->addWidget(WDFunc::NewLBLT(this, "Гистерезис фильтрации:", "", "", "Доля от диапазона, при превышении которой между замерами фильтр не работает"));
    hlyout->addWidget(WDFunc::NewSPB(this, "filterthr", 0, 1, 3, ACONFWCLR));
    hlyout->addWidget(WDFunc::NewLBLT(this, "Гистерезис уставок:", "", "", "Значение гистерезиса на сравнение с уставками (доля от диапазона)"));
    hlyout->addWidget(WDFunc::NewSPB(this, "hysteresis", 0, 1, 3, ACONFWCLR));
    hlyout->addStretch(10);
    gblyout->addLayout(hlyout);
    lyout->addLayout(gblyout);

    QWidget *cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFGCLR)+";}";
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
    glyout->addWidget(WDFunc::NewLBL(this, "№ канала", ACONFWCLR),0,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Диапазон", ACONFWCLRO),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Границы сигнала", ACONFWCLR),0,2,1,2);
    glyout->addWidget(WDFunc::NewLBL(this, "Мин. инж.", ACONFWCLRO),0,3,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Макс. инж.", ACONFWCLR),0,4,1,1);
    for (i = 0; i < Params.NumCh; ++i)
    {
        row = i+1;
        hlyout = new QHBoxLayout;
        glyout->addWidget(WDFunc::NewLBL(this, QString::number(i+1), ACONFWCLR),row,0,1,1);
        QComboBox *mcb = WDFunc::NewCB(this, "inrange."+QString::number(i), Params.RangeTypes, ACONFWCLRO);
        connect(mcb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetRange(int)));
        glyout->addWidget(mcb, row,1,1,1);
        hlyout->addWidget(WDFunc::NewSPB(this, "0."+QString::number(i), -20.0, 20.0, 2, ACONFWCLR));
        hlyout->addWidget(WDFunc::NewSPB(this, "1."+QString::number(i), -20.0, 20.0, 2, ACONFWCLR));
        hlyout->addWidget(WDFunc::NewLBL(this, "мА", ACONFWCLR, "units"));
        glyout->addLayout(hlyout, row, 2, 1, 1);
        glyout->addWidget(WDFunc::NewSPB(this, "2."+QString::number(i), -100000.0, 100000.0, 2, ACONFWCLRO),row,3,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "3."+QString::number(i), -100000.0, 100000.0, 2, ACONFWCLR),row,4,1,1);
    }
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFGCLR)+";}";
    cp->setStyleSheet(tmps);
    cp->setLayout(lyout);
    ConfTW->addTab(cp,"Диапазоны");

    lyout = new QVBoxLayout;
    gb = new QGroupBox("Уставки");
    glyout = new QGridLayout;
    glyout->setColumnStretch(0,0);
    glyout->setColumnStretch(1,5);
    glyout->setColumnStretch(2,5);
    glyout->setColumnStretch(3,5);
    glyout->setColumnStretch(4,5);
    glyout->addWidget(WDFunc::NewLBL(this, "№ канала"),0,0,1,1,Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBL(this, "Мин. авар."),0,1,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Мин. пред."),0,2,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Макс. пред."),0,3,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Макс. авар."),0,4,1,1,Qt::AlignCenter);
    for (i = 0; i < Params.NumCh; ++i)
    {
        row = i+1;
        glyout->addWidget(WDFunc::NewLBL(this, QString::number(i+1)),row,0,1,1,Qt::AlignRight);
        glyout->addWidget(WDFunc::NewSPB(this, "4."+QString::number(i), -100000.0, 100000.0, 2, ACONFRCLR),row,1,1,1,Qt::AlignCenter); // setminmin
        glyout->addWidget(WDFunc::NewSPB(this, "5."+QString::number(i), -100000.0, 100000.0, 2, ACONFYCLR),row,2,1,1,Qt::AlignCenter); // setmin
        glyout->addWidget(WDFunc::NewSPB(this, "6."+QString::number(i), -100000.0, 100000.0, 2, ACONFYCLR),row,3,1,1,Qt::AlignCenter); // setmax
        glyout->addWidget(WDFunc::NewSPB(this, "7."+QString::number(i), -100000.0, 100000.0, 2, ACONFRCLR),row,4,1,1,Qt::AlignCenter); // setmaxmax
    }
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp->setStyleSheet(tmps);
    cp->setLayout(lyout);
    ConfTW->addTab(cp,"Уставки");

    lyout = new QVBoxLayout;
    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void ConfDialog21::DisableChannel(int chnum, bool disable)
{
    bool Enabled = !disable;
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

void ConfDialog21::SetRange(int RangeType)
{
    int ChNum = GetChNumFromObjectName(sender()->objectName());
    switch(RangeType)
    {
    case 0: // 4..20 мА
    case 1: // 0..20 мА
    case 2: // 0..5 мА
    case 5: // manual мА
        WDFunc::SetLBLText(this, "units", "мА");
        break;
    case 3: // -5..5 В
    case 4: // 0..5 В
    case 6: // manual В
        WDFunc::SetLBLText(this, "units", "В");
        break;
    default:
        return;
    }
    if (RangeType < 5) // predefined ranges
    {
        C21->Bci_block.in_min[ChNum] = RangeInMins.at(RangeType);
        C21->Bci_block.in_max[ChNum] = RangeInMaxs.at(RangeType);
    }
}

void ConfDialog21::Fill()
{
    int i;
    WDFunc::SetSPBData(this, "oscdly", C21->Bci_block.oscdly);
    WDFunc::SetSPBData(this, "oscthr", C21->Bci_block.oscthr);
    WDFunc::SetSPBData(this, "filtern", C21->Bci_block.filtern);
    WDFunc::SetSPBData(this, "filterthr", C21->Bci_block.filterthr);
    WDFunc::SetSPBData(this, "hysteresis", C21->Bci_block.hysteresis);
    for (i = 0; i < AIN21_NUMCH; i++)
    {
        WDFunc::SetCBIndex(this, "chtypcb."+QString::number(i), C21->Bci_block.in_type[i]);
        quint8 Osc = C21->Bci_block.osc[i];
        WDFunc::SetChBData(this, "choscdi2."+QString::number(i), Osc&0x20);
        WDFunc::SetChBData(this, "choscdi1."+QString::number(i), Osc&0x10);
        WDFunc::SetChBData(this, "choscthr."+QString::number(i), Osc&0x04);
        WDFunc::SetChBData(this, "chosccso0."+QString::number(i), Osc&0x01);
        SetMinMax(i);
        WDFunc::SetSPBData(this, "2."+QString::number(i), C21->Bci_block.in_vmin[i]); // in_vmin
        WDFunc::SetSPBData(this, "3."+QString::number(i), C21->Bci_block.in_vmax[i]); // in_vmax
        WDFunc::SetSPBData(this, "4."+QString::number(i), C21->Bci_block.setminmin[i]); // setminmin
        WDFunc::SetSPBData(this, "5."+QString::number(i), C21->Bci_block.setmin[i]); // setmin
        WDFunc::SetSPBData(this, "6."+QString::number(i), C21->Bci_block.setmax[i]); // setmax
        WDFunc::SetSPBData(this, "7."+QString::number(i), C21->Bci_block.setmaxmax[i]); // setmaxmax
        DisableChannel(i, (C21->Bci_block.in_type[i] == AIN21_AIT_NONE));
    }
}

void ConfDialog21::FillBack()
{
    WDFunc::SPBData(this, "oscdly", C21->Bci_block.oscdly);
    WDFunc::SPBData(this, "oscthr", C21->Bci_block.oscthr);
    WDFunc::SPBData(this, "filtern", C21->Bci_block.filtern);
    WDFunc::SPBData(this, "filterthr", C21->Bci_block.filterthr);
    WDFunc::SPBData(this, "hysteresis", C21->Bci_block.hysteresis);
    for (int i=0; i<AIN21_NUMCH; ++i)
    {
        WDFunc::CBIndex(this, "chtypcb."+QString::number(i), C21->Bci_block.in_type[i]);
        bool tmpb;
        quint8 Osc = 0;
        WDFunc::ChBData(this, "choscdi2."+QString::number(i), tmpb);
        if (tmpb) Osc |= 0x20;
        WDFunc::ChBData(this, "choscdi1."+QString::number(i), tmpb);
        if (tmpb) Osc |= 0x10;
        WDFunc::ChBData(this, "choscthr."+QString::number(i), tmpb);
        if (tmpb) Osc |= 0x04;
        WDFunc::ChBData(this, "chosccso0."+QString::number(i), tmpb);
        if (tmpb) Osc |= 0x01;
        C21->Bci_block.osc[i] = Osc;
        WDFunc::SPBData(this, "2."+QString::number(i), C21->Bci_block.in_vmin[i]); // in_vmin
        WDFunc::SPBData(this, "3."+QString::number(i), C21->Bci_block.in_vmax[i]); // in_vmax
        WDFunc::SPBData(this, "4."+QString::number(i), C21->Bci_block.setminmin[i]); // setminmin
        WDFunc::SPBData(this, "5."+QString::number(i), C21->Bci_block.setmin[i]); // setmin
        WDFunc::SPBData(this, "6."+QString::number(i), C21->Bci_block.setmax[i]); // setmax
        WDFunc::SPBData(this, "7."+QString::number(i), C21->Bci_block.setmaxmax[i]); // setmaxmax
    }
}

void ConfDialog21::SetMinMax(int i)
{
    QComboBox *cb = this->findChild<QComboBox *>("inrange."+QString::number(i));
    if (cb == 0)
    {
        DBGMSG;
        return;
    }
    switch (C21->Bci_block.in_type[i])
    {
    case AIN21_AIT_MA: // канал с мА
    {
        if (pc.FloatInRange(C21->Bci_block.in_min[i],0.0) && pc.FloatInRange(C21->Bci_block.in_max[i],20.0))
            cb->setCurrentIndex(RT_I020);
        else if (pc.FloatInRange(C21->Bci_block.in_min[i],4.0) && pc.FloatInRange(C21->Bci_block.in_max[i],20.0))
            cb->setCurrentIndex(RT_I420);
        else if (pc.FloatInRange(C21->Bci_block.in_min[i],0.0) && pc.FloatInRange(C21->Bci_block.in_max[i],5.0))
            cb->setCurrentIndex(RT_I05);
        else
            cb->setCurrentIndex(RT_IMANUAL);
        break;
    }
    case AIN21_AIT_V: // канал с В
    {
        if (pc.FloatInRange(C21->Bci_block.in_min[i],0.0) && pc.FloatInRange(C21->Bci_block.in_max[i],5.0))
            cb->setCurrentIndex(RT_V05);
        else if (pc.FloatInRange(C21->Bci_block.in_min[i], -5.0) && pc.FloatInRange(C21->Bci_block.in_max[i],5.0))
            cb->setCurrentIndex(RT_V55);
        else
            cb->setCurrentIndex(RT_VMANUAL);
        break;
    }
    default:
        break;
    }
}

void ConfDialog21::SetChTypData(int value)
{
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == GENERALERROR)
        return;
    C21->Bci_block.in_type[tmpi] = value;
    DisableChannel(tmpi, (value == 0));
}

void ConfDialog21::CheckConf()
{
    for (int i=0; i<Params.NumCh; ++i)
    {
        if (C21->Bci_block.in_min[i] > C21->Bci_block.in_max[i])
            CheckConfErrors.append("IN_MIN["+QString::number(i)+"] > IN_MAX["+QString::number(i)+"]");
        if (C21->Bci_block.in_vmin[i] > C21->Bci_block.in_vmax[i])
            CheckConfErrors.append("IN_VMIN["+QString::number(i)+"] > IN_VMAX["+QString::number(i)+"]");
        if (C21->Bci_block.setminmin[i] > C21->Bci_block.setmin[i])
            CheckConfErrors.append("SETMINMIN["+QString::number(i)+"] > SETMIN["+QString::number(i)+"]");
        if (C21->Bci_block.setminmin[i] < C21->Bci_block.in_vmin[i])
            CheckConfErrors.append("SETMINMIN["+QString::number(i)+"] < IN_VMIN["+QString::number(i)+"]");
        if (C21->Bci_block.setmin[i] < C21->Bci_block.in_vmin[i])
            CheckConfErrors.append("SETMIN["+QString::number(i)+"] < IN_VMIN["+QString::number(i)+"]");
        if (C21->Bci_block.setmax[i] < C21->Bci_block.setmin[i])
            CheckConfErrors.append("SETMAX["+QString::number(i)+"] < SETMIN["+QString::number(i)+"]");
        if (C21->Bci_block.setmax[i] > C21->Bci_block.setmaxmax[i])
            CheckConfErrors.append("SETMAX["+QString::number(i)+"] > SETMAXMAX["+QString::number(i)+"]");
        if (C21->Bci_block.setmax[i] > C21->Bci_block.in_vmax[i])
            CheckConfErrors.append("SETMAX["+QString::number(i)+"] > IN_VMAX["+QString::number(i)+"]");
        if (C21->Bci_block.setmaxmax[i] > C21->Bci_block.in_vmax[i])
            CheckConfErrors.append("SETMAXMAX["+QString::number(i)+"] > IN_VMAX["+QString::number(i)+"]");
    }
}

void ConfDialog21::SetDefConf()
{
    C21->SetDefConf();
}
