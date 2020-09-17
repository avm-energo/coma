#ifndef AVARALARMKTF_H
#define AVARALARMKTF_H

#include "../alarm/abstractalarm.h"

class AvarAlarmKTF : public AbstractAvarAlarm
{
    Q_OBJECT

public:
    explicit AvarAlarmKTF(AlarmClass *alarm, QDialog *parent = nullptr);

public slots:

    void AvarAlarmState() override;

private:
};

#endif // AVARALARMKTF_H
