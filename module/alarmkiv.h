#ifndef AVARALARMKIV_H
#define AVARALARMKIV_H

#include "alarm.h"

class AlarmKIV : public Alarm
{
    Q_OBJECT

public:
    explicit AlarmKIV(QObject *parent = nullptr);
};

#endif // AVARALARM84_H