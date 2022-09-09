#include "basealarm.h"

BaseAlarm::BaseAlarm(QWidget *parent) : UWidget(parent), normalColor(Qt::transparent), alarmColor(Qt::red)
{
}

void BaseAlarm::disable()
{
    disconnect(this);
}
