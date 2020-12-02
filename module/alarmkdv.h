#pragma once

#include "alarm.h"

class CritKDV : public BaseAlarm
{
    Q_OBJECT

public:
    explicit CritKDV(QWidget *parent = nullptr);
};

class WarnKDV : public BaseAlarm
{
    Q_OBJECT

public:
    explicit WarnKDV(QWidget *parent = nullptr);
};
