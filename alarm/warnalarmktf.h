#ifndef WARNALARMKTF_H
#define WARNALARMKTF_H

#include "../alarm/abstractalarm.h"

class WarnAlarmKTF : public AbstractAlarm
{
    Q_OBJECT

public:
    explicit WarnAlarmKTF(AlarmClass *alarm, QDialog *parent = nullptr);
    int j = 0;
public slots:

    void AlarmState();
    void WarnAlarmState();
    void AvarState();

    void Update(QList<bool> states);

private:
};

#endif // PREDALARMKTF_H
