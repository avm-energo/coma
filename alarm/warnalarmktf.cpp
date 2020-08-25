#include "warnalarmktf.h"

#include "../gen/colors.h"
#include "../gen/commands.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
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

WarnAlarmKTF::WarnAlarmKTF(AlarmClass *alarm, QWidget *parent) : AbstractAlarm(parent)
{
    Alarm = alarm;
    WarnAlarmState();
}

void WarnAlarmKTF::WarnAlarmState()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    const QStringList events = QStringList() << "Отсутствует сигнал напряжения фазы A                   "
                                             << "Отсутствует сигнал напряжения фазы B                   "
                                             << "Отсутствует сигнал напряжения фазы С                   "
                                             << "Отсутствует сигнал тока  фазы А                        "
                                             << "Отсутствует сигнал тока  фазы B                        "
                                             << "Отсутствует сигнал тока  фазы C                        "
                                             << "Перегрузка по току фазы A                              "
                                             << "Перегрузка по току фазы В                              "
                                             << "Перегрузка по току фазы С                              "
                                             << "Машина включена                                        "
                                             << "Сигнализация по опасному превышению температуры обмотки"
                                             << "Неисправны все датчики температуры обмотки             "
                                             << "Сигнализация по опасному уровню пускового тока         ";
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};"); // color:
                                                                            // rgba(220,220,220,255);

    for (int i = 0; i < Alarm->MapAlarm[MTYPE_KTF].warnCounts; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb, SIGNAL(clicked()), this, SLOT(close()));
    lyout->addWidget(pb, 0);
    setLayout(lyout);
}

void WarnAlarmKTF::AlarmState()
{
}

void WarnAlarmKTF::AvarState()
{
}

void WarnAlarmKTF::Update(QList<bool> states)
{
    int i = 0;

    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");

    for (i = 0; i < Alarm->MapAlarm[MTYPE_KTF].warnCounts; i++)
    {
        quint32 alarm = states[i];
        WDFunc::SetLBLImage(this, (QString::number(i)), (alarm) ? pmred : pmgrn);
    }
}
