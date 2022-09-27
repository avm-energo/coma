#pragma once

#include "../widgets/uwidget.h"

constexpr int circleRadius = 15;

class BaseAlarm : public UWidget
{
    Q_OBJECT
public:
    explicit BaseAlarm(QWidget *parent = nullptr);

protected:
    QColor normalColor;
    QColor alarmColor;

    virtual void setupUI(const QStringList &events) = 0;

signals:
    void updateAlarm(const bool &value);
    void updateColor(const QColor &color);
};
