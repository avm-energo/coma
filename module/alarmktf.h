#pragma once

#include "alarm.h"

class CritKTF : public BaseAlarm
{
    Q_OBJECT

public:
    explicit CritKTF(QWidget *parent = nullptr);
};

class WarnKTF : public BaseAlarm
{
    Q_OBJECT

public:
    explicit WarnKTF(QWidget *parent = nullptr);
};
