// check21.cpp
#include <QGridLayout>
#include <QGroupBox>
#include "check21.h"
#include "../publicclass.h"
#include "../widgets/wd_func.h"

Check21::Check21()
{

}

QWidget *Check21::BdaW(QWidget *parent)
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    for (int i = 0; i < 16; i++)
    {
        hlyout->addWidget(WDFunc::NewLBLT(parent, QString::number(i)+":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT(parent, "", "bda"+QString::number(i), ValuesFormat), 1);
        if ((i>1)&&!((i+1)%6))
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

QWidget *Check21::Bd1W(QWidget *parent)
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QGroupBox *gb = new QGroupBox(title);
    QGridLayout *glyout = new QGridLayout;
    int i;
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lbl = new QLabel("Тек. измерения, приведённые ко входу (ток)");
    hlyout->addWidget(lbl,0);
    hlyout->addWidget(line,1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    for (i = 0; i < 16; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT(this, QString::number(i)+":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT(this, "", "bdinI"+QString::number(i), ValuesFormat), 1);
        if ((i>0)&&!((i+1)%6))
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
    lbl = new QLabel("Тек. измерения, приведённые ко входу (напряжения)");
    hlyout->addWidget(lbl,0);
    hlyout->addWidget(line,1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    for (i = 0; i < 16; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT(this, QString::number(i)+":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT(this, "", "bdinU"+QString::number(i), ValuesFormat), 1);
        if ((i>0)&&!((i+1)%6))
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
    lbl = new QLabel("Тек. измерения в инженерных единицах");
    hlyout->addWidget(lbl,0);
    hlyout->addWidget(line,1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    for (i = 0; i < 16; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT(this, QString::number(i)+":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT(this, "", "bdain"+QString::number(i), ValuesFormat), 1);
        if ((i>0)&&!((i+1)%6))
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
    lbl = new QLabel("Текущие минимумы по каналам");
    hlyout->addWidget(lbl,0);
    hlyout->addWidget(line,1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    for (i = 0; i < 16; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT(this, QString::number(i)+":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT(this, "", "bdainn"+QString::number(i), ValuesFormat), 1);
        if ((i>0)&&!((i+1)%6))
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
    lbl = new QLabel("Текущие максимумы по каналам");
    hlyout->addWidget(lbl,0);
    hlyout->addWidget(line,1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    for (i = 0; i < 16; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT(this, QString::number(i)+":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT(this, "", "bdainx"+QString::number(i), ValuesFormat), 1);
        if ((i>0)&&!((i+1)%6))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    if (hlyout->count())
        vlyout->addLayout(hlyout);
    lyout->addLayout(vlyout);
    hlyout->addWidget(WDFunc::NewLBLT(this, "Температура в кристалле:"), 0);
    hlyout->addWidget(WDFunc::NewLBLT(this, "", "bdat", ValuesFormat), 1);
    vlyout->addLayout(hlyout);
    vlyout->addStretch(100);
    w->setLayout(vlyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

void Check21::FillBda(QWidget *parent)
{
    int i;
    for (i = 0; i < 16; ++i)
        WDFunc::SetLEData(parent, "bda"+QString::number(i), QString::number(Bda_block.sin[i]/ACHECKNUMPOINTS,16));
}

void Check21::FillBd1W(QWidget *parent)
{
    int i;
    for (i = 0; i < 16; ++i)
    {
        QString tmps = QString::number(i);
        WDFunc::SetLEData(parent, "bdinI"+tmps, QString::number(Bd_block.inI[i],'f',5));
        WDFunc::SetLEData(parent, "bdinU"+tmps, QString::number(Bd_block.inU[i],'f',5));
        WDFunc::SetLEData(parent, "bdain"+tmps, QString::number(Bd_block.ain[i],'f',5));
        WDFunc::SetLEData(parent, "bdainn"+tmps, QString::number(Bd_block.ainn[i],'f',5));
        WDFunc::SetLEData(parent, "bdainx"+tmps, QString::number(Bd_block.ainx[i],'f',5));
    }
    WDFunc::SetLEData(parent, "bdat", QString::number(Bd_block.at,'f',5));
}