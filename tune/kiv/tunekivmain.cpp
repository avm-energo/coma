#include "tunekivmain.h"

TuneKIVMain::TuneKIVMain(TuneKIV *kiv, QWidget *parent) : AbstractTuneDialog(1, parent)
{
    TKIV = kiv;
    //    m_tuneStep = 1;
    SetBac(&TKIV->m_Bac, 0, sizeof(TKIV->m_Bac));
    SetupUI();
}

void TuneKIVMain::SetupUI()
{
}

void TuneKIVMain::setMessages()
{
}

void TuneKIVMain::setTuneFunctions()
{
}

void TuneKIVMain::FillBac(int bacnum)
{
    Q_UNUSED(bacnum)
}

void TuneKIVMain::FillBackBac(int bacnum)
{
    Q_UNUSED(bacnum)
}

void TuneKIVMain::GetBdAndFill()
{
}

int TuneKIVMain::ReadAnalogMeasurements()
{
    return 0;
}

void TuneKIVMain::SetDefCoefs()
{
}
