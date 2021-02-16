#pragma once

#include "../module/module.h"

#include <QObject>
class TuneModule : public Module
{
    Q_OBJECT
public:
    explicit TuneModule(QObject *parent = nullptr);
    void create(QTimer *updateTimer) override;
signals:

    // Module interface
protected:
    void create(Modules::Model model) override;
};
