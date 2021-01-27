#pragma once

#include "modulealarm.h"

class CritKTF : public ModuleAlarm
{
    Q_OBJECT

public:
    explicit CritKTF(QWidget *parent = nullptr);
};

class WarnKTF : public ModuleAlarm
{
    Q_OBJECT

public:
    explicit WarnKTF(QWidget *parent = nullptr);
};
