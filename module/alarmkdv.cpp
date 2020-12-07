#include "alarmkdv.h"

#include "modules.h"
CritKDV::CritKDV(QWidget *parent) : BaseAlarm(parent)
{
    m_startAlarmAddress = 5011;
    m_alarmFlags = std::bitset<32>(AVM_KDV::criticalFlags);

    setupUI(AVM_KDV::criticalEvents);
}

WarnKDV::WarnKDV(QWidget *parent) : BaseAlarm(parent)
{
    m_startAlarmAddress = 3011;
    m_alarmFlags = std::bitset<32>(AVM_KDV::warningFlags);

    setupUI(AVM_KDV::warningEvents);
}
