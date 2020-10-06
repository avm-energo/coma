#ifndef AVARALARMKIV_H
#define AVARALARMKIV_H

#include "alarm.h"

#define KIVALARMCOUNT 7

class AlarmKIV : public Alarm
{
    Q_OBJECT

public:
    explicit AlarmKIV(QWidget *parent = nullptr);
};

#endif // AVARALARM84_H
