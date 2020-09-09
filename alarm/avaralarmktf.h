#ifndef AVARALARMKTF_H
#define AVARALARMKTF_H

#include "../alarm/abstractalarm.h"

class AvarAlarmKTF : public AbstractAlarm
{
    Q_OBJECT

public:
    explicit AvarAlarmKTF(AlarmClass *alarm, QDialog *parent = nullptr);
    int j = 0;
public slots:

    void AlarmState();
    void WarnAlarmState();
    void AvarState();

    void Update(QList<bool> states);

private:
};

#endif // AVARALARMKTF_H
