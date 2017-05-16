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
#include "../canal.h"

AbstractConfDialog3x::AbstractConfDialog3x(QWidget *parent) :
    AbstractConfDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void AbstractConfDialog3x::SetupUI()
{
    int i;
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
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
    QStringList dlysl = QStringList() << "0,5" << "2" << "4" << "16";
    for (i = 0; i < Params.NumCh; ++i)
    {
        lbl = new QLabel(QString::number(i+1)+":");
        gblyout->addWidget(lbl, i+1, 0, 1, 1, Qt::AlignRight);
        s_tqComboBox *cb = WDFunc::NewCB(this, "chtypcb."+QString::number(i), Params.InTypes, ACONFWCLR);
        connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetChTypData()));
        gblyout->addWidget(cb, i+1, 1, 1, 1, Qt::AlignRight);
        s_tqComboBox *spb = WDFunc::NewCB(this, "chdlycb."+QString::number(i), dlyslm, ACONFWCLR);

    }
    gb->setLayout(gblyout);
    lyout->addWidget(gb);
    QWidget *cp = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFGCLR)+";}";
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
    WDFunc::SetEnabled(this, "choscdi1."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "choscdi2."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "choscthr."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "chosccso0."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "inrange."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "0."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "1."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "2."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "3."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "4."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "5."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "6."+QString::number(chnum), Enabled);
    WDFunc::SetEnabled(this, "7."+QString::number(chnum), Enabled);
}
