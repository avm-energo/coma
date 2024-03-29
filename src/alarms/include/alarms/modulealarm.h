#pragma once

#include <QLabel>
#include <QList>
#include <QPair>
#include <alarms/basealarm.h>
#include <gen/datatypes.h>

/// \brief The class for alarm dialog, that displaying critical,
/// warning and info alarms from XML configuration files.
class ModuleAlarm final : public BaseAlarm
{
    Q_OBJECT
private:
    const AlarmValue m_alarms;
    QList<QPair<QLabel *, bool>> m_labelStateStorage;

    /// \brief Setup UI: creating text labels and indicators (pixmaps) for alarms displaying.
    void setupUI(const QStringList &events);
    /// \brief Check if all pixmap labels inactive.
    bool isAllPixmapInactive() const;
    /// \brief Update a indicator (pixmap) for alarms displaying.
    void updatePixmap(const bool &isSet, const quint32 &position);

public:
    explicit ModuleAlarm(const AlarmType type, const AlarmValue &alarms, //
        Device::CurrentDevice *device, QWidget *parent = nullptr);

    /// \brief Folowing the data: search a signal group whose range
    /// includes the address of the first alarm from the list.
    void followToData(const SignalMap &sigMap);

public slots:
    /// \brief This slot called when a SinglePoint data is received from the device.
    void updateSPData(const DataTypes::SinglePointWithTimeStruct &sp) override;
};
