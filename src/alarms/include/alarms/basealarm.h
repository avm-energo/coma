#pragma once

#include "../../widgets/uwidget.h"

constexpr int circleRadius = 15; ///< Indicator's circle radius.

/// \brief Abstract base class for alarm's dialogs.
/// \see AlarmStateAll, ModuleAlarm.
class BaseAlarm : public UWidget
{
    Q_OBJECT
public:
    explicit BaseAlarm(Device::CurrentDevice *device, QWidget *parent = nullptr);

    /// \brief Setter for the alarm color.
    void setAlarmColor(const QColor &newAlarmColor) noexcept;

    /// \brief Setter for the normal color.
    void setNormalColor(const QColor &newNormalColor) noexcept;

protected:
    QColor m_normalColor;
    QColor m_alarmColor;

    /// \details Pure virtual function for UI setup.
    virtual void setupUI(const QStringList &events) = 0;

signals:
    /// \details This signal is emitted when indicator color is changed.
    void updateColor(const QColor &color);
};
