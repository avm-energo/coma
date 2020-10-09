#include "warnalarmktf.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"

#include <QBoxLayout>

WarnAlarmKTF::WarnAlarmKTF(AlarmClass *alarm, QWidget *parent) : AbstractWarnAlarm(parent)
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
                                             //                                             << "Машина включена "
                                             << "Сигнализация по опасному превышению температуры обмотки"
                                             << "Неисправны все датчики температуры обмотки             "
                                             << "Сигнализация по опасному уровню пускового тока         ";
    SetupAlarm(events, (Alarm->MapAlarm.value(Board::GetInstance().type()).warnCounts) - 1);
}
