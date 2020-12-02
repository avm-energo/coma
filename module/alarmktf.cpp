#include "alarmktf.h"

#include "modules.h"

CritKTF::CritKTF(QWidget *parent) : BaseAlarm(parent)
{
    m_startAlarmAddress = 5011;
    m_alarmFlags = std::bitset<32>(AVM_KTF::criticalFlags);

    setupUI(AVM_KTF::criticalEvents);
}

WarnKTF::WarnKTF(QWidget *parent) : BaseAlarm(parent)
{
    m_startAlarmAddress = 5011;
    m_alarmFlags = std::bitset<32>(AVM_KTF::warningFlags);

    setupUI(AVM_KTF::warningEvents);
}
