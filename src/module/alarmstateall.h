#ifndef ALARMSTATEALL_H
#define ALARMSTATEALL_H

#define BSI_HEALTH_MASK 0xFFFF
#include "../module/basealarm.h"

class AlarmStateAll : public BaseAlarm
{
    Q_OBJECT
public:
    explicit AlarmStateAll(QWidget *parent = nullptr);
    void reqUpdate() override;
    void update(quint32 health);
    void setupUI(const QStringList &events) override;

signals:

private:
};

#endif // ALARMSTATE_H
