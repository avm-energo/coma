#ifndef AVARALARMKDV_H
#define AVARALARMKDV_H

#include "alarm.h"

class AlarmKDV : public Alarm
{
    Q_OBJECT

public:
    explicit AlarmKDV(QWidget *parent = nullptr);
};

#endif // AVARALARMKDV_H
