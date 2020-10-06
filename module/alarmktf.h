#ifndef AVARALARMKTF_H
#define AVARALARMKTF_H

#include "alarm.h"

class AlarmKTF : public Alarm
{
    Q_OBJECT

public:
    explicit AlarmKTF(QWidget *parent = nullptr);
};

#endif // AVARALARMKTF_H
