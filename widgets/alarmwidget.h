#ifndef ALARMWIDGET_H
#define ALARMWIDGET_H

#include "../module/basealarm.h"
#include "../widgets/uwidget.h"
//#define BSIWARNMASK 0x00005F55
//#define BSIALARMMASK 0x000020AA

class AlarmWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmWidget(QWidget *parent = nullptr);
    void clear();

    void addAlarm(BaseAlarm *alarm);
    int count() const;

public slots:

    void UpdateIndicator(bool);

private:
    QList<BaseAlarm *> m_alarms;
    // QDialogButtonBox *buttons;
    QTimer *m_timer;
    const QStringList buttonDescription {
        "Состояние устройства",           //
        "Предупредительная сигнализация", //
        "Аварийная сигнализация"          //
    };
    int m_counter = 0;
    //    AlarmClass *Alarm;
};

#endif // ALARMWIDGET_H
