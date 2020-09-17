#ifndef WARNALARMKIV_H
#define WARNALARMKIV_H

#include "../alarm/abstractalarm.h"

class WarnAlarmKIV : public AbstractWarnAlarm
{
    Q_OBJECT

public:
    explicit WarnAlarmKIV(AlarmClass *alarm, QWidget *parent = nullptr);

public slots:

    void WarnAlarmState() override;

private:
};

#endif // PREDALARM84_H
