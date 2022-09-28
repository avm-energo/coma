#pragma once

#include "../widgets/uwidget.h"

constexpr int circleRadius = 15; ///< Indicator's circle radius.

/// \brief Abstract base class for alarm's dialogs.
/// \see AlarmStateAll, ModuleAlarm.
class BaseAlarm : public UWidget
{
    Q_OBJECT
public:
    explicit BaseAlarm(QWidget *parent = nullptr) : UWidget(parent), normalColor(Qt::transparent), alarmColor(Qt::red)
    {
    }

protected:
    QColor normalColor;
    QColor alarmColor;

    /// \details Pure virtual function for UI setup.
    virtual void setupUI(const QStringList &events) = 0;

signals:
    /// \details This signal is emitted when indicator color is changed.
    void updateColor(const QColor &color);
};
