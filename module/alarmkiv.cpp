#include "alarmkiv.h"

#include "module_kiv.h"
CritKIV::CritKIV(QWidget *parent) : BaseAlarm(parent)
{
    m_startAlarmAddress = 3011;
    m_alarmFlags = std::bitset<32>(AVM_KIV::criticalFlags);

    setupUI(AVM_KIV::criticalEvents);
}

WarnKIV::WarnKIV(QWidget *parent) : BaseAlarm(parent)
{
    m_startAlarmAddress = 3011;
    m_alarmFlags = std::bitset<32>(AVM_KIV::warningFlags);

    setupUI(AVM_KIV::warningEvents);
}
