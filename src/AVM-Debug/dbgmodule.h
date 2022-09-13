#pragma once

#include "../module/module.h"

#include <QObject>
class DbgModule : public QObject
{
    Q_OBJECT
public:
    explicit DbgModule(QObject *parent = nullptr);
    void create(QTimer *updateTimer);

protected:
    void createModule(Modules::Model model);

private:
    void create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM);
    void createUSIO(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM);
};
