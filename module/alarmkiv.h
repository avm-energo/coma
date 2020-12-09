#pragma once

#include "modulealarm.h"

class CritKIV : public ModuleAlarm
{
    Q_OBJECT

public:
    explicit CritKIV(QWidget *parent = nullptr);
};

class WarnKIV : public ModuleAlarm
{
    Q_OBJECT

public:
    explicit WarnKIV(QWidget *parent = nullptr);
};
