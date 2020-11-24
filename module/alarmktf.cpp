#include "alarmktf.h"

#include "modules.h"

AlarmKTF::AlarmKTF(QWidget *parent) : Alarm(parent)
{
    m_alarmBdNum = 2;
    m_startAlarmAddress = 5011;
    m_alarmFlags = std::bitset<32>(AVM_KTF::accidentFlags);

    setupUI(AVM_KTF::accidentEvents);
}
