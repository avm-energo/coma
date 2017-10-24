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
#include "../widgets/messagebox.h"
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

    QGroupBox *gb = new QGroupBox("Свойства каналов");
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
    for (i=0; i<Params.NumCh; ++i)
        chsl << QString::number(i);
    for (i = 0; i < Params.NumCh; ++i)
    {
        lbl = new QLabel(QString::number(i+1)+":");
        gblyout->addWidget(lbl, i+1, 0, 1, 1, Qt::AlignRight);
        s_tqComboBox *cb = WDFunc::NewCB(this, "chtypcb."+QString::number(i), Params.InTypes, DCONFWCLR);
        connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetChTypData(int)));
        gblyout->addWidget(cb, i+1, 1, 1, 1, Qt::AlignCenter);
        QDoubleSpinBox *spb = WDFunc::NewSPB(this, "chdlyspb."+QString::number(i),1,20,1,0,DCONFWCLR);
//        connect(spb,SIGNAL(valueChanged(double)),this,SLOT(SetDly(double)));
        gblyout->addWidget(spb, i+1, 2, 1, 1, Qt::AlignCenter);
        cb = WDFunc::NewCB(this, "chpaircb."+QString::number(i), chsl, DCONFWCLR);
        connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetPair(int)));
        gblyout->addWidget(cb, i+1, 3, 1, 1, Qt::AlignCenter);
    }
    gb->setLayout(gblyout);
    lyout->addWidget(gb);
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

void AbstractConfDialog3x::DisableChannel(int chnum, bool disable)
{
    bool Enabled = !disable;
    WDFunc::SetEnabled(this, "chtypcb."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "chdlycb."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "chpaircb."+QString::number(chnum), Enabled);
}
