#pragma once

#include "alarm.h"

class CritKIV : public BaseAlarm
{
    Q_OBJECT

public:
    explicit CritKIV(QWidget *parent = nullptr);
};

class WarnKIV : public BaseAlarm
{
    Q_OBJECT

public:
    explicit WarnKIV(QWidget *parent = nullptr);
};
