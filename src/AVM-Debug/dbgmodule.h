#pragma once

#include "../module/module.h"

#include <QObject>
class DbgModule : public Module
{
    Q_OBJECT
public:
    explicit DbgModule(QObject *parent = nullptr);
    void create(QTimer *updateTimer) override;
    using Module::create;

protected:
    void createModule(Modules::Model model) override;

private:
    void create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM) override;
    void createUSIO(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM);
};
