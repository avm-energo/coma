#ifndef ALARMWIDGET_H
#define ALARMWIDGET_H

#include <QWidget>

class BaseAlarm;

class AlarmWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmWidget(QWidget *parent = nullptr);
    void clear();

    void addAlarm(BaseAlarm *alarm, const QString caption);
    int count() const;
    void disableAlarms();

public slots:
    void updateIndicator(bool);

private:
    QList<BaseAlarm *> m_alarms;
    QTimer *m_timer;
    int m_counter = 0;
};

#endif // ALARMWIDGET_H
