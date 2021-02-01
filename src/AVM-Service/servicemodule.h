#pragma once

#include "../module/module.h"

#include <QObject>
class ServiceModule : public Module
{
    Q_OBJECT
public:
    explicit ServiceModule(QObject *parent = nullptr);
    ServiceModule(QTimer *updateTimer, AlarmWidget *aw, QObject *parent = nullptr) : Module(updateTimer, aw, parent)
    {
    }

signals:

    // Module interface
protected:
    void create(Modules::Model model) override;
};
