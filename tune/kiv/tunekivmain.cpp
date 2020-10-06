#include "tunekivmain.h"

TuneKIVMain::TuneKIVMain(TuneKIV *kiv, QWidget *parent) : AbstractTuneDialog(parent)
{
    TKIV = kiv;
    m_tuneStep = 1;
    addTuneBlock(&TKIV->m_Bac_block, 0, sizeof(TKIV->m_Bac_block));
    SetupUI();
}

void TuneKIVMain::SetupUI()
{
}

void TuneKIVMain::SetLbls()
{
}

void TuneKIVMain::SetPf()
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
