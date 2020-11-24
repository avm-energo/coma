#include "alarmkdv.h"

#include "modules.h"
AlarmKDV::AlarmKDV(QWidget *parent) : Alarm(parent)
{
    m_alarmBdNum = 2;
    m_startAlarmAddress = 5011;
    m_alarmFlags = std::bitset<32>(AVM_KDV::accidentFlags);

    setupUI(AVM_KDV::accidentEvents);
}
