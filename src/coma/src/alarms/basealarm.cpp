#include "alarms/basealarm.h"

BaseAlarm::BaseAlarm(Device::CurrentDevice *device, QWidget *parent)
    : UWidget(device, parent), m_normalColor(Qt::transparent), m_alarmColor(Qt::red)
{
}

void BaseAlarm::setAlarmColor(const QColor &newAlarmColor) noexcept
{
    m_alarmColor = newAlarmColor;
}

void BaseAlarm::setNormalColor(const QColor &newNormalColor) noexcept
{
    m_normalColor = newNormalColor;
}

QColor BaseAlarm::getColorByType(const AlarmType type) const noexcept
{
    switch (type)
    {
    case AlarmType::Critical:
        return Qt::red;
    case AlarmType::Warning:
        return Qt::yellow;
    case AlarmType::Info:
        return Qt::green;
    default:
        return Qt::transparent;
    }
}
