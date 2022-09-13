#pragma once

#include "../module/module.h"

#include <QObject>
class SvcModule : QObject
{
    Q_OBJECT
public:
    explicit SvcModule(QObject *parent = nullptr);
    void create(QTimer *updateTimer);

protected:
    void createModule(Modules::Model model);

private:
    void create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM);
};
