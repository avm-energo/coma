#include "alarmkiv.h"

AlarmKIV::AlarmKIV(QWidget *parent) : Alarm(parent)
{
    m_alarmBdNum = 11;
    m_startAlarmAddress = 3011;
    m_alarmFlags = std::bitset<32>(0x00000e39);
    QStringList events = QStringList() << "Авария по приращению тангенса дельта ввода фазы А"
                                       << "Авария по приращению тангенса дельта ввода фазы B"
                                       << "Авария по приращению тангенса дельта ввода фазы C"
                                       << "Авария по приращению C ввода фазы А              "
                                       << "Авария по приращению C ввода фазы B              "
                                       << "Авария по приращению C ввода фазы C              "
                                       << "Авария по недопустимому небалансу токов          ";
    setupUI(events);
}
