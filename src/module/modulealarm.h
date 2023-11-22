#pragma once

#include "basealarm.h"
#include "modulesettings.h"

#include <QLabel>
#include <QList>
#include <QPair>
#include <gen/datatypes.h>

/// \brief The class for alarm dialog, that displaying critical,
/// warning and info alarms from XML configuration files.
class ModuleAlarm final : public BaseAlarm
{
    Q_OBJECT
private:
    static const std::map<Modules::AlarmType, QColor> s_colors;
    const ModuleTypes::AlarmValue m_alarms;
    QList<QPair<QLabel *, bool>> m_labelStateStorage;

    /// \brief Setup UI: creating text labels and indicators (pixmaps) for alarms displaying.
    virtual void setupUI(const QStringList &events) override;
    /// \brief Folowing the data: search a signal group whose range
    /// includes the address of the first alarm from the list.
    void followToData();
    /// \brief Check if all pixmap labels inactive.
    bool isAllPixmapInactive() const;
    /// \brief Update a indicator (pixmap) for alarms displaying.
    void updatePixmap(const bool &isSet, const quint32 &position);

public:
    explicit ModuleAlarm(const Modules::AlarmType &type, //
        const ModuleTypes::AlarmValue &alarms, QWidget *parent = nullptr);

public slots:
    /// \brief This slot called when a SinglePoint data is received from the device.
    void updateSPData(const DataTypes::SinglePointWithTimeStruct &sp) override;
};
