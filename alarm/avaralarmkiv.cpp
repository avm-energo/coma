
#include "avaralarmkiv.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"
#include "abstractalarm.h"

#include <QBoxLayout>

AvarAlarmKIV::AvarAlarmKIV(AlarmClass *alarm, QWidget *parent) : AbstractAvarAlarm(parent)
{
    Alarm = alarm;
    AvarAlarmState();
}

void AvarAlarmKIV::AvarAlarmState()
{

    QStringList events = QStringList() << "Авария по приращению тангенса дельта ввода фазы А"
                                       << "Авария по приращению тангенса дельта ввода фазы B"
                                       << "Авария по приращению тангенса дельта ввода фазы C"
                                       << "Авария по приращению C ввода фазы А              "
                                       << "Авария по приращению C ввода фазы B              "
                                       << "Авария по приращению C ввода фазы C              "
                                       << "Авария по недопустимому небалансу токов          ";
    SetupAlarm(events, Alarm->MapAlarm.value(Board::GetInstance()->type()).avarCounts);
}
