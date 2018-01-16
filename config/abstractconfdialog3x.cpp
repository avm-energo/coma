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
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QTabBar>
#include "abstractconfdialog3x.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

AbstractConfDialog3x::AbstractConfDialog3x(QWidget *parent) :
    AbstractConfDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void AbstractConfDialog3x::SetupUI()
{
    int i;
    QString tmps = "QDialog {background-color: "+QString(DCONFCLR)+";}";
    setStyleSheet(tmps);
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    QVBoxLayout *lyout = new QVBoxLayout;

    if (Params.InNumCh > 0)
    {
        QGroupBox *gb = new QGroupBox("Входные каналы");
        QGridLayout *gblyout = new QGridLayout;
        QLabel *lbl = new QLabel("#");
        gblyout->addWidget(lbl, 0, 0, 1, 1, Qt::AlignRight);
        lbl = new QLabel("Тип канала");
        gblyout->addWidget(lbl, 0, 1, 1, 1, Qt::AlignCenter);
        lbl = new QLabel("Задержка на дребезг, мс");
        gblyout->addWidget(lbl, 0, 2, 1, 1, Qt::AlignCenter);
        lbl = new QLabel("Парный канал");
        gblyout->addWidget(lbl, 0, 3, 1, 1, Qt::AlignCenter);
        QStringList chsl = QStringList() << "X"; // "x" - нет пары
        for (i=0; i<Params.InNumCh; ++i)
            chsl << QString::number(i+1);
        for (i = 0; i < Params.InNumCh; ++i)
        {
            lbl = new QLabel(QString::number(i+1)+":");
            gblyout->addWidget(lbl, i+1, 0, 1, 1, Qt::AlignRight);
            EComboBox *cb = WDFunc::NewCB(this, "inchtypcb."+QString::number(i), Params.InTypes, DCONFWCLR);
            connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetChTypData(int)));
            gblyout->addWidget(cb, i+1, 1, 1, 1, Qt::AlignCenter);
            QDoubleSpinBox *spb = WDFunc::NewSPB(this, "chdlyspb."+QString::number(i),1,20,1,0,DCONFWCLR);
            connect(spb,SIGNAL(valueChanged(double)),this,SLOT(SetDly(double)));
            gblyout->addWidget(spb, i+1, 2, 1, 1, Qt::AlignCenter);
            cb = WDFunc::NewCB(this, "chpaircb."+QString::number(i), chsl, DCONFWCLR);
            connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetPair(int)));
            gblyout->addWidget(cb, i+1, 3, 1, 1, Qt::AlignCenter);
        }
        gb->setLayout(gblyout);
        lyout->addWidget(gb);
    }
    if (Params.OutNumCh > 0)
    {
        QGroupBox *gb = new QGroupBox("Выходные каналы");
        QGridLayout *gblyout = new QGridLayout;
        int col = 0;
        gblyout->addWidget(WDFunc::NewLBL(this, "#"), 0, col++, 1, 1, Qt::AlignRight);
        gblyout->addWidget(WDFunc::NewLBL(this, "Тип канала"), 0, col++, 1, 1, Qt::AlignCenter);
        for (i = 0; i < Params.OutNumCh; ++i)
        {
            col = 0;
            gblyout->addWidget(WDFunc::NewLBL(this, QString::number(i)+":"), i+1, col++, 1, 1, Qt::AlignRight);
            EComboBox *cb = WDFunc::NewCB(this, "outchtypcb."+QString::number(i), Params.OutTypes, DCONFWCLR);
            connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetChTypData(int)));
            gblyout->addWidget(cb, i+1, col++, 1, 1);
        }
        ++i;
        gblyout->addWidget(WDFunc::NewLBL(this, "Мин. импульс, мс"), i, 0, 1, 1, Qt::AlignRight);
        gblyout->addWidget(WDFunc::NewSPB(this, "minimp", 50, 1000, 1, 0, DCONFWCLR), i++, 1, 1, 1);
        gblyout->addWidget(WDFunc::NewLBL(this, "Макс. импульс, мс"), i, 0, 1, 1, Qt::AlignRight);
        gblyout->addWidget(WDFunc::NewSPB(this, "maximp", 50, 1000, 1, 0, DCONFWCLR), i++, 1, 1, 1);
        gblyout->addWidget(WDFunc::NewLBL(this, "Тип Watchdog"), i, 0, 1, 1, Qt::AlignRight);
        gblyout->addWidget(WDFunc::NewSPB(this, "wdgtype", 0, 255, 1, 0, DCONFWCLR), i++, 1, 1, 1);
        gblyout->addWidget(WDFunc::NewLBL(this, "Номер выхода для Watchdog"), i, 0, 1, 1, Qt::AlignRight);
        QStringList tmpsl;
        for (int j=0; j<Params.OutNumCh; ++j)
            tmpsl << QString::number(j+1);
        gblyout->addWidget(WDFunc::NewCB(this, "wdgout", tmpsl, DCONFWCLR), i++, 1, 1, 1);
        gblyout->addWidget(WDFunc::NewLBL(this, "Время Watchdog, с"), i, 0, 1, 1, Qt::AlignRight);
        gblyout->addWidget(WDFunc::NewSPB(this, "wdgtime", 0, 255, 1, 0, DCONFWCLR), i++, 1, 1, 1);
        gblyout->setColumnStretch(1, 10);
        gb->setLayout(gblyout);
        lyout->addWidget(gb);
    }
    QWidget *cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(DCONFGCLR)+";}";
    cp->setStyleSheet(tmps);
    cp->setLayout(lyout);
    ConfTW->addTab(cp,"Общие");

    lyout = new QVBoxLayout;
    lyout->addWidget(ConfTW);
    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void AbstractConfDialog3x::SetInputs(QStringList InputsTypes, int InputsNum)
{
    Params.InNumCh = InputsNum;
    Params.InTypes = InputsTypes;
}

void AbstractConfDialog3x::SetOutputs(QStringList OutputsTypes, int OutputsNum)
{
    Params.OutNumCh = OutputsNum;
    Params.OutTypes = OutputsTypes;
}

void AbstractConfDialog3x::DisableChannel(int chnum, bool disable)
{
    bool Enabled = !disable;
    WDFunc::SetEnabled(this, "chtypcb."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "chdlycb."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "chpaircb."+QString::number(chnum), Enabled);
}
