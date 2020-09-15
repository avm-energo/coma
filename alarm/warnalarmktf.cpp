#include "warnalarmktf.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"

#include <QBoxLayout>

WarnAlarmKTF::WarnAlarmKTF(AlarmClass *alarm, QDialog *parent) : AbstractWarnAlarm(parent)
{
    Alarm = alarm;
    WarnAlarmState();
}

void WarnAlarmKTF::WarnAlarmState()
{

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
    QVBoxLayout *lyout = new QVBoxLayout;

    QVBoxLayout *vlayout = new QVBoxLayout;
    for (int i = 0; i < Alarm->MapAlarm[MTYPE_KTF].warnCounts; ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "transparent", QString::number(i)));
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
