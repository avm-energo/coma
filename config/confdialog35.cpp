#include "confdialog35.h"
#include "config35.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

ConfDialog35::ConfDialog35(QVector<publicclass::DataRec> &S2Config, bool BaseBoard, QWidget *parent) :
    AbstractConfDialog3x(parent)
{
    this->S2Config = &S2Config;
    C35 = new Config35(S2Config, BaseBoard);
    SetInputs(QStringList(), 0); // нет входов
    QStringList sl = QStringList() << "Не исп." << "Обычный";
    SetOutputs(sl, D35_NUMCH);
    SetupUI();
    PrereadConf();
}

void ConfDialog35::Fill()
{
    for (int i=0; i<D35_CHNUM; ++i)
        WDFunc::SetCBIndex(this, "outchtypcb."+QString::number(i), pc.IndexByBit(C35->Bci_block.out_type[i]));
    WDFunc::SetSPBData(this, "minimp", C35->Bci_block.pulse_short);
    WDFunc::SetSPBData(this, "maximp", C35->Bci_block.pulse_long);
    WDFunc::SetSPBData(this, "wdgtype", C35->Bci_block.wd_type);
    WDFunc::SetCBIndex(this, "wdgout", C35->Bci_block.wd_nout);
    WDFunc::SetSPBData(this, "wdgtime", C35->Bci_block.wd_tm);
    CheckConf();
}

void ConfDialog35::FillBack()
{
    quint32 tmpi;
    for (int i=0; i<D35_CHNUM; ++i)
    {
        WDFunc::CBIndex(this, "outchtypcb."+QString::number(i), tmpi);
        C35->Bci_block.out_type[i] = pc.BitByIndex(tmpi);
    }
    WDFunc::SPBData(this, "minimp", C35->Bci_block.pulse_short);
    WDFunc::SPBData(this, "maximp", C35->Bci_block.pulse_long);
    WDFunc::SPBData(this, "wdgtype", C35->Bci_block.wd_type);
    WDFunc::CBIndex(this, "wdgout", C35->Bci_block.wd_nout);
    WDFunc::SPBData(this, "wdgtime", C35->Bci_block.wd_tm);
}

void ConfDialog35::CheckConf()
{
    if (C35->Bci_block.pulse_long < C35->Bci_block.pulse_short)
        CheckConfErrors.append("Ширина длинного импульса короче, чем ширина короткого");
}

void ConfDialog35::SetDefConf()
{
    C35->SetDefConf();
    Fill();
}

void ConfDialog35::SetChTypData(int value)
{
    Q_UNUSED(value);
}

void ConfDialog35::SetDly(double dly)
{
    Q_UNUSED(dly);
}

void ConfDialog35::SetPair(int ch)
{
    Q_UNUSED(ch);
}
