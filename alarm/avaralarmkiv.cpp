
#include "avaralarmkiv.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"
#include "abstractalarm.h"

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

AvarAlarmKIV::AvarAlarmKIV(AlarmClass *alarm, QWidget *parent) : AbstractAlarm(parent)
{
    Alarm = alarm;
    AvarState();
}

void AvarAlarmKIV::AvarState()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);

    QStringList events = QStringList() << "Авария по приращению тангенса дельта ввода фазы А"
                                       << "Авария по приращению тангенса дельта ввода фазы B"
                                       << "Авария по приращению тангенса дельта ввода фазы C"
                                       << "Авария по приращению C ввода фазы А              "
                                       << "Авария по приращению C ввода фазы B              "
                                       << "Авария по приращению C ввода фазы C              "
                                       << "Авария по недопустимому небалансу токов          ";
    QWidget *w = new QWidget;

    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};"); // color: rgba(220,220,220,255);

    for (int i = 0; i < Alarm->MapAlarm[MTYPE_KIV].avarCounts; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb, SIGNAL(clicked()), dlg, SLOT(close()));
    lyout->addWidget(pb, 0);
    dlg->setLayout(lyout);
    dlg->show();
}

void AvarAlarmKIV::AlarmState()
{
}
void AvarAlarmKIV::WarnAlarmState()
{
}

void AvarAlarmKIV::Update(QList<bool> states)
{
    int i = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");

    for (i = 0; i < Alarm->MapAlarm[MTYPE_KIV].avarCounts; i++)
    {
        quint32 alarm = states[i];
        WDFunc::SetLBLImage(this, QString::number(i), (alarm) ? pmred : pmgrn);
    }
}
