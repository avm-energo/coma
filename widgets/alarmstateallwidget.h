#ifndef ALARMSTATEALL_H
#define ALARMSTATEALL_H

#define CIRCLE_RADIUS 15
#define BSI_HEALTH_MASK 0xFFFF
#include <QWidget>

class AlarmStateAll : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmStateAll(QWidget *parent = nullptr);

public slots:

    //    void AlarmState();
    void UpdateHealth(quint32 health);
    //    void CallUpdateHealth();

    void update();

signals:
    void BSIUpdated(bool isset);

private:
    const QList<int> WarnPositions { 1, 3, 5, 7, 13 };
};

#endif // ALARMSTATE_H
