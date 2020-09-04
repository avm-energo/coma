
#include "warnalarmkiv.h"

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

WarnAlarmKIV::WarnAlarmKIV(AlarmClass *alarm, QDialog *parent) : AbstractAlarm(parent)
{
    Alarm = alarm;
    WarnAlarmState();
}

void WarnAlarmKIV::WarnAlarmState()
{
    // QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    const QStringList events = QStringList() << "Отсутствует сигнал напряжения фазы A                   "
                                             << "Отсутствует сигнал напряжения фазы B                   "
                                             << "Отсутствует сигнал напряжения фазы С                   "
                                             << "Отсутствует ток ввода фазы А (ток меньше 2мА)          "
                                             << "Отсутствует ток ввода фазы B (ток меньше 2мА)          "
                                             << "Отсутствует ток ввода фазы C (ток меньше 2мА)          "
                                             << "Не заданы начальные значения                           "
                                             << "Низкое напряжение фазы A                               "
                                             << "Низкое напряжение фазы B                               "
                                             << "Низкое напряжение фазы C                               "
                                             << "Сигнализация по приращению тангенса дельта ввода фазы А"
                                             << "Сигнализация по приращению тангенса дельта ввода фазы B"
                                             << "Сигнализация по приращению тангенса дельта ввода фазы C"
                                             << "Сигнализация по приращению C ввода фазы А              "
                                             << "Сигнализация по приращению C ввода фазы B              "
                                             << "Сигнализация по приращению C ввода фазы C              "
                                             << "Не заданы паспортные значения                          "
                                             << "Сигнализация по повышенному небалансу токов            ";
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");

    for (int i = 0; i < Alarm->MapAlarm[MTYPE_KIV].warnCounts; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb, SIGNAL(clicked()), this, SLOT(hide()));
    lyout->addWidget(pb, 0);
    setLayout(lyout);
}

void WarnAlarmKIV::AlarmState()
{
}
void WarnAlarmKIV::AvarState()
{
}

void WarnAlarmKIV::Update(QList<bool> states)
{
    int i = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    if (states.isEmpty())
        return;
    for (i = 0; i < Alarm->MapAlarm[MTYPE_KIV].warnCounts; i++)
    {
        quint32 alarm = states.at(i);
        WDFunc::SetLBLImage(this, (QString::number(i)), (alarm) ? pmred : pmgrn);
    }
}
