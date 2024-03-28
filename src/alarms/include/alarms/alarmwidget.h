#pragma once

#include <QWidget>
#include <alarms/alarmstateall.h>
#include <alarms/modulealarm.h>

namespace Device
{
class CurrentDevice;
}

/// \brief Alarm widget with buttons for alarms dialogs.
/// \see AlarmStateAll, ModuleAlarm.
class AlarmWidget : public QWidget
{
    Q_OBJECT
private:
    QList<BaseAlarm *> m_alarms;
    QTimer *m_timer;
    int m_counter = 0;

    void addAlarm(BaseAlarm *alarm, const QString caption);

public:
    explicit AlarmWidget(QWidget *parent = nullptr);
    void configure(Device::CurrentDevice *device);
    void clear();

public slots:
    void updateAlarmOperation(const bool mode) noexcept;
    void updateAlarmInterval(const int interval) noexcept;
};
