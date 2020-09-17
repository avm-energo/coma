#include "avaralarmktf.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"

#include <QBoxLayout>

AvarAlarmKTF::AvarAlarmKTF(AlarmClass *alarm, QDialog *parent) : AbstractAvarAlarm(parent)
{
    Alarm = alarm;
    AvarAlarmState();
}

void AvarAlarmKTF::AvarAlarmState()
{

    QStringList events = QStringList() << "Аварийное сообщение по недопустимому превышению температуры обмотки"
                                       << "Аварийное сообщение по недопустимому уровню пускового тока         ";

    SetupAlarm(events, Alarm->MapAlarm.value(Board::GetInstance()->type()).avarCounts);
}
