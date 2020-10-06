#include "alarm.h"

#include "../gen/board.h"

#include <QtDebug>

Alarm::Alarm(QWidget *parent) : QWidget(parent) { }

void Alarm::Update(QList<bool> states)
{
    if (states.isEmpty())
        return;
    auto max_range = states.length();
    if (m_alarmFlags.size() != states.length())
    {
        qDebug() << m_alarmFlags.size() << ":" << states.length();
        max_range = std::min(m_alarmFlags.size(), states.length());
    }
    for (int i = 0; i < max_range; i++)
    {
        UpdatePixmaps(states.at(i), i);
    }
}
