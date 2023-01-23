#pragma once

#include "basealarm.h"
#include "modulesettings.h"

#include <QLabel>
#include <QList>
#include <QPair>
#include <gen/datatypes.h>

/// \brief The class for alarm dialog, that displaying critical,
/// warning and info alarms from XML configuration files.
class ModuleAlarm : public BaseAlarm
{
    Q_OBJECT
private:
    static const QHash<Modules::AlarmType, QColor> colors;
    const ModuleTypes::AlarmValue mAlarms;
    UniquePointer<DataTypesProxy> mProxy;
    QList<QPair<QLabel *, bool>> labelStateStorage;

    virtual void setupUI(const QStringList &events) override;
    void followToData();
    bool isAllPixmapInactive() const;
    void updatePixmap(const bool &isSet, const quint32 &position);

public:
    explicit ModuleAlarm(const Modules::AlarmType &type, //
        const ModuleTypes::AlarmValue &alarms, QWidget *parent = nullptr);

public slots:
    void update(const QVariant &msg);
};
