#pragma once

#include "../module/module.h"

#include <QObject>
class TuneModule : public Module
{
    Q_OBJECT
public:
    explicit TuneModule(QObject *parent = nullptr);
    void create(QTimer *updateTimer) override;
    using Module::create;

protected:
    void createModule(Modules::Model model) override;

private:
    void create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM) override;
};
