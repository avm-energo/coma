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
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"
#include "../canal.h"

ConfDialog21::ConfDialog21(QVector<publicclass::DataRec> &S2Config, bool BaseBoard, QWidget *parent) :
    AbstractConfDialog2x(parent)
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
        C21->Bci_block.inblk.in_min[ChNum] = RangeInMins.at(RangeType);
        C21->Bci_block.inblk.in_max[ChNum] = RangeInMaxs.at(RangeType);
    }
}

void ConfDialog21::Fill()
{
    int i;
    WDFunc::SetSPBData(this, "oscdlyspb", C21->Bci_block.inblk.oscdly);
    for (i = 0; i < AIN21_NUMCH; i++)
    {
        WDFunc::SetCBIndex(this, "chtypcb."+QString::number(i), C21->Bci_block.inblk.in_type[i]);
        quint8 Osc = C21->Bci_block.inblk.osc[i];
        WDFunc::SetChBData(this, "choscdi2."+QString::number(i), Osc&0x20);
        WDFunc::SetChBData(this, "choscdi1."+QString::number(i), Osc&0x10);
        WDFunc::SetChBData(this, "choscthr."+QString::number(i), Osc&0x04);
        WDFunc::SetChBData(this, "chosccso0."+QString::number(i), Osc&0x01);
        SetMinMax(i);
        WDFunc::SetSPBData(this, "2."+QString::number(i), C21->Bci_block.inblk.in_vmin[i]); // in_vmin
        WDFunc::SetSPBData(this, "3."+QString::number(i), C21->Bci_block.inblk.in_vmax[i]); // in_vmax
        WDFunc::SetSPBData(this, "4."+QString::number(i), C21->Bci_block.inblk.setminmin[i]); // setminmin
        WDFunc::SetSPBData(this, "5."+QString::number(i), C21->Bci_block.inblk.setmin[i]); // setmin
        WDFunc::SetSPBData(this, "6."+QString::number(i), C21->Bci_block.inblk.setmax[i]); // setmax
        WDFunc::SetSPBData(this, "7."+QString::number(i), C21->Bci_block.inblk.setmaxmax[i]); // setmaxmax
        DisableChannel(i, (C21->Bci_block.inblk.in_type[i] == Config2x::AIT_NONE));
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
    switch (C21->Bci_block.inblk.in_type[i])
    {
    case Config2x::AIT_MA: // канал с мА
    {
        if (pc.FloatInRange(C21->Bci_block.inblk.in_min[i],0.0) && pc.FloatInRange(C21->Bci_block.inblk.in_max[i],20.0))
            cb->setCurrentIndex(RT_I020);
        else if (pc.FloatInRange(C21->Bci_block.inblk.in_min[i],4.0) && pc.FloatInRange(C21->Bci_block.inblk.in_max[i],20.0))
            cb->setCurrentIndex(RT_I420);
        else if (pc.FloatInRange(C21->Bci_block.inblk.in_min[i],0.0) && pc.FloatInRange(C21->Bci_block.inblk.in_max[i],5.0))
            cb->setCurrentIndex(RT_I05);
        else
            cb->setCurrentIndex(RT_IMANUAL);
        break;
    }
    case Config2x::AIT_V: // канал с В
    {
        if (pc.FloatInRange(C21->Bci_block.inblk.in_min[i],0.0) && pc.FloatInRange(C21->Bci_block.inblk.in_max[i],5.0))
            cb->setCurrentIndex(RT_V05);
        else if (pc.FloatInRange(C21->Bci_block.inblk.in_min[i], -5.0) && pc.FloatInRange(C21->Bci_block.inblk.in_max[i],5.0))
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
    C21->Bci_block.inblk.in_type[tmpi] = value;
    DisableChannel(tmpi, (value == 0));
}

void ConfDialog21::SetOscDly(double dly)
{
    C21->Bci_block.inblk.oscdly = static_cast<quint16>(dly);
}

void ConfDialog21::SetChOsc(int isChecked)
{
    int ChNum = GetChNumFromObjectName(sender()->objectName());
    if (ChNum == -1)
        return;
    int Mask;
    QString ObjName = sender()->objectName().split(".").at(0);
    if (ObjName.compare("choscdi1") == 0)
        Mask = 0x10;
    if (ObjName.compare("choscdi2") == 0)
        Mask = 0x20;
    if (ObjName.compare("choscthr") == 0)
        Mask = 0x04;
    if (ObjName.compare("chosccso0") == 0)
        Mask = 0x10;
    if (isChecked == Qt::Checked)
        C21->Bci_block.inblk.osc[ChNum] |= Mask;
    else
        C21->Bci_block.inblk.osc[ChNum] &= ~Mask;
}

bool ConfDialog21::CheckConf()
{
    bool NotGood = false;
    for (int i=0; i<Params.NumCh; ++i)
    {
        if (C21->Bci_block.inblk.in_min[i] > C21->Bci_block.inblk.in_max[i])
            NotGood = true;
        if (C21->Bci_block.inblk.in_vmin[i] > C21->Bci_block.inblk.in_vmax[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setminmin[i] > C21->Bci_block.inblk.setmin[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setminmin[i] < C21->Bci_block.inblk.in_vmin[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setmin[i] < C21->Bci_block.inblk.in_vmin[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setmax[i] < C21->Bci_block.inblk.setmin[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setmax[i] > C21->Bci_block.inblk.setmaxmax[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setmax[i] > C21->Bci_block.inblk.in_vmax[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setmaxmax[i] > C21->Bci_block.inblk.in_vmax[i])
            NotGood = true;
    }
    return NotGood;

}

void ConfDialog21::SetDefConf()
{
    C21->SetDefConf();
}

void ConfDialog21::SetIn()
{
    s_tqSpinBox *spb = qobject_cast<s_tqSpinBox *>(sender());
    QString ObjectName = spb->objectName().split(".").at(0);
    int idx = ObjectName.toInt();
    if (idx == GENERALERROR)
    {
        DBGMSG;
        return;
    }
    int chnum = GetChNumFromObjectName(sender()->objectName());
    if (chnum <= 0)
    {
        DBGMSG;
        return;
    }
    QString tmps;
    switch(idx)
    {
    case 0: // inmin
        C21->Bci_block.inblk.in_min[chnum] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        break;
    case 1: // inmax
        C21->Bci_block.inblk.in_max[chnum] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        break;
    case 2: // invmin
        C21->Bci_block.inblk.in_vmin[chnum] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        break;
    case 3: // invmax
        C21->Bci_block.inblk.in_vmax[chnum] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        break;
    case 4: // setminmin
        C21->Bci_block.inblk.setminmin[chnum] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFRCLR)+";}";
        break;
    case 5: // setmin
        C21->Bci_block.inblk.setmin[chnum] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFYCLR)+";}";
        break;
    case 6: // setmax
        C21->Bci_block.inblk.setmax[chnum] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFYCLR)+";}";
        break;
    case 7: // setmaxmax
        C21->Bci_block.inblk.setmaxmax[chnum] = spb->value();
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFRCLR)+";}";
        break;
    }
    if (CheckConf())
        tmps = "QDoubleSpinBox {color: "+QString(ERRCLR)+"; font: bold;}";
    spb->setStyleSheet(tmps);
}
