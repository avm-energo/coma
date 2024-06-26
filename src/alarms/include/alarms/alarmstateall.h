#pragma once

#include <alarms/basealarm.h>

/// \brief The class for alarm dialog, that displaying the device general status.
class AlarmStateAll final : public BaseAlarm
{
    Q_OBJECT
private:
    AlarmStateAllConfig m_config;
    QColor m_strongestColor;

    /// \brief Setup UI: creating text labels and indicators (pixmaps) for alarms displaying.
    void setupUI();

    /// \brief Generating default alarm widget configuration.
    void generateDefaultConfig();

    /// \brief Update current strongest color for alarm.
    void updateStrongestColor(const QColor &newColor);

    /// \brief Request for updating a BSI data.
    void reqUpdate() override;

public:
    explicit AlarmStateAll(Device::CurrentDevice *device, QWidget *parent = nullptr);

public slots:
    /// \brief The slot called when a device health changed for updating alarms.
    void update(const quint32 health);
};
