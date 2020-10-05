#ifndef WARNALARMKTF_H
#define WARNALARMKTF_H

#include "../alarm/abstractalarm.h"

class WarnAlarmKTF : public AbstractWarn
{
    Q_OBJECT

public:
    explicit WarnAlarmKTF(AlarmClass *alarm, QDialog *parent = nullptr);

public slots:

    void WarnAlarmState() override;

private:
};

#endif // PREDALARMKTF_H
