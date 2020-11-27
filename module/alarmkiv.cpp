#include "alarmkiv.h"

#include "module_kiv.h"
#include "modules.h"
AlarmKIV::AlarmKIV(QWidget *parent) : Alarm(parent)
{
    m_alarmBdNum = 11;
    m_startAlarmAddress = 3011;
    m_alarmFlags = std::bitset<32>(AVM_KIV::accidentFlags);

    setupUI(AVM_KIV::accidentEvents);
}
