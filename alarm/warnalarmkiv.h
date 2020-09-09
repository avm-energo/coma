#ifndef WARNALARMKIV_H
#define WARNALARMKIV_H

#include "../alarm/abstractalarm.h"

class WarnAlarmKIV : public AbstractAlarm
{
    Q_OBJECT

public:
    explicit WarnAlarmKIV(AlarmClass *alarm, QDialog *parent = nullptr);

    int j = 0;

public slots:

    void AlarmState();
    void WarnAlarmState();
    void AvarState();
    void Update(QList<bool> states);

private:
};

#endif // PREDALARM84_H
