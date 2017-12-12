#include "confdialog35.h"
#include "config35.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

ConfDialog35::ConfDialog35(QVector<publicclass::DataRec> &S2Config, bool BaseBoard, QWidget *parent) :
    AbstractConfDialog3x(parent)
{
    this->S2Config = &S2Config;
    C35 = new Config35(S2Config, BaseBoard);
    SetupUI();
    PrereadConf();
}

void ConfDialog35::Fill()
{
    CheckConf();
}

void ConfDialog35::FillBack()
{

}

void ConfDialog35::CheckConf()
{
}

void ConfDialog35::SetDefConf()
{
    C35->SetDefConf();
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
