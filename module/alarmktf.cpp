#include "alarmktf.h"

AlarmKTF::AlarmKTF(QWidget *parent) : Alarm(parent)
{
    m_alarmBdNum = 2;
    m_startAlarmAddress = 5011;
    m_alarmFlags = std::bitset<32>(0x00000005);
    QStringList events = QStringList() << "Аварийное сообщение по недопустимому превышению температуры обмотки"
                                       << "Аварийное сообщение по недопустимому уровню пускового тока         ";
    setupUI(events);
}
