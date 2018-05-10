// check22.cpp
#include <QGridLayout>
#include <QGroupBox>
#include "check22.h"
#include "../gen/publicclass.h"
#include "../widgets/wd_func.h"

Check22::Check22()
{

}

QWidget *Check22::BdaW(QWidget *parent)
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    for (int i = 0; i < AIN22_NUMCH; i++)
    {
        hlyout->addWidget(WDFunc::NewLBLT(parent, QString::number(i)+":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT(parent, "", "bda"+QString::number(i), ValuesFormat), 1);
        if ((i>1)&&!((i+1)%4))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    if (hlyout->count())
        vlyout->addLayout(hlyout);
    vlyout->addStretch(100);
    w->setLayout(vlyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

QWidget *Check22::Bd1W(QWidget *parent)
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    int i;
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    QLabel *lbl = new QLabel("Тек. измерения в град. Цельсия");
    hlyout->addWidget(lbl,0);
    hlyout->addWidget(line,1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    for (i = 0; i < AIN22_NUMCH; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT(parent, QString::number(i)+":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT(parent, "", "tgr"+QString::number(i), ValuesFormat), 1);
        if ((i>0)&&!((i+1)%4))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    if (hlyout->count())
        vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lbl = new QLabel("Тек. измерения в Омах");
    hlyout->addWidget(lbl,0);
    hlyout->addWidget(line,1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    for (i = 0; i < AIN22_NUMCH; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT(parent, QString::number(i)+":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT(parent, "", "tom"+QString::number(i), ValuesFormat), 1);
        if ((i>0)&&!((i+1)%4))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    if (hlyout->count())
        vlyout->addLayout(hlyout);
    vlyout->addStretch(100);
    w->setLayout(vlyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

void Check22::FillBda(QWidget *parent)
{
    int i;
    for (i = 0; i < AIN22_NUMCH; ++i)
        WDFunc::SetLBLText(parent, "bda"+QString::number(i), QString::number(Bda_block.sin[i]));
}

void Check22::FillBd1W(QWidget *parent)
{
    int i;
    for (i = 0; i < AIN22_NUMCH; ++i)
    {
        QString tmps = QString::number(i);
        WDFunc::SetLBLText(parent, "tgr"+tmps, QString::number(Bd_block.Dmed[i],'f',5));
        WDFunc::SetLBLText(parent, "tom"+tmps, QString::number(Bd_block.Tmed[i],'f',5));
    }
}