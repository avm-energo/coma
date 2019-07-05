// check31.cpp
#include <QGridLayout>
#include <QGroupBox>
#include "check31.h"
#include "../gen/colors.h"
#include "../widgets/wd_func.h"

Check31::Check31()
{

}

QWidget *Check31::BdaW(QWidget *parent)
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QWidget *w = new QWidget(parent);
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    for (int i = 0; i < A31_NUMCH; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT(parent, QString::number(i)+":"), 0);
        hlyout->addWidget(WDFunc::NewLBL(parent, "", "", "bda" + QString::number(i), pmgrn), 1);
        if ((i>0)&&!((i+1)%8))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    if (hlyout->count())
        vlyout->addLayout(hlyout);
    w->setLayout(vlyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

void Check31::FillBda(QWidget *parent)
{
    int i;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmred, pmgrn};

    for (i = 0; i < A31_NUMCH; ++i)
    {
        int signal = ((Bda_block.chdata & (0x00000001 << i)) ? 1 : 0);
        WDFunc::SetLBLImage(parent, "bda"+QString::number(i), pm[signal]);
    }
}
