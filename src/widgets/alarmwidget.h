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

    void addAlarm(BaseAlarm *alarm);
    void addAlarm(BaseAlarm *alarm, const QString caption);
    int count() const;
    void disableAlarm();

public slots:

    void UpdateIndicator(bool);

private:
    QList<BaseAlarm *> m_alarms;

    QTimer *m_timer;
    const QStringList buttonDescription {
        "Состояние устройства",           //
        "Предупредительная сигнализация", //
        "Аварийная сигнализация"          //
    };
    int m_counter = 0;
};

#endif // ALARMWIDGET_H
