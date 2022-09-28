#pragma once

#include "../module/alarmstateall.h"
#include "../module/modulealarm.h"

#include <QWidget>

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
    void configure();
    void disableAlarms();
    void clear();
};
