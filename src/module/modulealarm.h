#ifndef ALARM_H
#define ALARM_H

#include "../gen/datatypes.h"
#include "basealarm.h"
#include "modulesettings.h"

class ModuleAlarm : public BaseAlarm
{
    Q_OBJECT
private:
    const ModuleTypes::AlarmValue &mAlarms;
    UniquePointer<DataTypesProxy> mProxy;

    virtual void setupUI(const QStringList &events) override;
    void followToData();
    void updatePixmap(const bool &isset, const quint32 &position);

public:
    explicit ModuleAlarm(const Modules::AlarmType &type, //
        const ModuleTypes::AlarmValue &alarms, QWidget *parent = nullptr);

public slots:
    void update(const QVariant &msg);
};

#endif // ALARM_H
