#pragma once

#include "../module/module.h"

#include <QObject>
class SvcModule : public Module
{
    Q_OBJECT
public:
    explicit SvcModule(QObject *parent = nullptr);
    void create(QTimer *updateTimer) override;

protected:
    void createModule(Modules::Model model) override;

private:
    void create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM) override;
};
