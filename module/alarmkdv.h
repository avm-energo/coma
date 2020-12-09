#pragma once

#include "modulealarm.h"

class CritKDV : public ModuleAlarm
{
    Q_OBJECT

public:
    explicit CritKDV(QWidget *parent = nullptr);
};

class WarnKDV : public ModuleAlarm
{
    Q_OBJECT

public:
    explicit WarnKDV(QWidget *parent = nullptr);
};
