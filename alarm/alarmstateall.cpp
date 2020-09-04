#include "alarmstateall.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"

#include <QApplication>
#include <QBoxLayout>
#include <QCursor>
#include <QDialog>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QStringListModel>
#include <QToolBar>

AlarmStateAll::AlarmStateAll(QDialog *parent) : AbstractAlarm(parent)
{
    AlarmState();
}

void AlarmStateAll::AlarmState()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->setObjectName("hlyout");
    QVBoxLayout *vlayout = new QVBoxLayout;

    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QWidget *w = new QWidget;
    w->setObjectName("window");
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};"); // color:
                                                                            // rgba(220,220,220,255);

    for (int i = 0; i < HthToolTip().size(); ++i)
    {
        hlyout = new QHBoxLayout;

        if (ModuleBSI::ModuleBsi.Hth & (0x00000001 << i))
            hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i), pmred));

        else
            hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i), pmgrn));

        hlyout->addWidget(WDFunc::NewLBLT(w, HthToolTip().at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb, SIGNAL(clicked()), this, SLOT(hide()));
    lyout->addWidget(pb, 0);
    this->setLayout(lyout);
}
void AlarmStateAll::WarnAlarmState()
{
}
void AlarmStateAll::AvarState()
{
}
void AlarmStateAll::Update(QList<bool>)
{
}

void AlarmStateAll::UpdateHealth(quint32 health)
{
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QHBoxLayout *hlyout = new QHBoxLayout;
    for (quint32 i = 0; i < health; ++i)
    {
        hlyout = new QHBoxLayout;

        if (health & (0x00000001 << i))
            WDFunc::SetLBLImage(this, QString::number(i), pmred);
        else
            WDFunc::SetLBLImage(this, QString::number(i), pmgrn);
    }
}

void AlarmStateAll::CallUpdateHealth()
{
    this->UpdateHealth(ModuleBSI::ModuleBsi.Hth);
}
