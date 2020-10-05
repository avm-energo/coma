#ifndef WARNALARMKIV_H
#define WARNALARMKIV_H

#include "../alarm/abstractalarm.h"

class WarnKIV : public AbstractWarn
{
    Q_OBJECT

public:
    explicit WarnKIV(AlarmClass *alarm, QWidget *parent = nullptr);

public slots:

    void WarnAlarmState() override;

private:
};

#endif // PREDALARM84_H
