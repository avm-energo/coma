#ifndef AVARALARMKIV_H
#define AVARALARMKIV_H

#include "../alarm/abstractalarm.h"

class AvarAlarmKIV : public AbstractAlarm
{
    Q_OBJECT

public:
    explicit AvarAlarmKIV(AlarmClass *alarm, QDialog *parent = nullptr);
    int j = 0;

public slots:

    void AlarmState();
    void WarnAlarmState();
    void AvarState();
    void Update(QList<bool> states);

private:
};

#endif // AVARALARM84_H
