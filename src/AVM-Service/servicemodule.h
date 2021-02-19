#pragma once

#include "../module/module.h"

#include <QObject>
class ServiceModule : public Module
{
    Q_OBJECT
public:
    explicit ServiceModule(QObject *parent = nullptr);
    void create(QTimer *updateTimer) override;

protected:
    void create(Modules::Model model) override;

private:
    void create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM) override;
};
