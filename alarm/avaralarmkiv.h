#ifndef AVARALARMKIV_H
#define AVARALARMKIV_H

#include "../alarm/abstractalarm.h"

class AvarAlarmKIV : public AbstractAvarAlarm
{
    Q_OBJECT

public:
    explicit AvarAlarmKIV(AlarmClass *alarm, QWidget *parent = nullptr);

public slots:

    void AvarAlarmState() override;

private:
};

#endif // AVARALARM84_H
