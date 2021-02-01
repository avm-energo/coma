#pragma once

#include "../module/module.h"

#include <QObject>
class TuneModule : public Module
{
    Q_OBJECT
public:
    explicit TuneModule(QObject *parent = nullptr);
    TuneModule(QTimer *updateTimer, AlarmWidget *aw, QObject *parent = nullptr) : Module(updateTimer, aw, parent)
    {
    }

signals:
};
