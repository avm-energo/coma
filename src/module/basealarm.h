#pragma once

#include "../interfaces/baseinterface.h"

#include <QWidget>
#include <bitset>
constexpr int circleRadius = 15;
class BaseAlarm : public QWidget
{

    Q_OBJECT
public:
    explicit BaseAlarm(QWidget *parent = nullptr);

    void disable();
public slots:
    virtual void reqUpdate() = 0;

protected:
    std::bitset<128> m_alarmFlags;       // '1' equals alarm
    std::bitset<128> m_actualAlarmFlags; // '1' equals alarm

    QColor m_normalColor;
    QColor m_alarmColor;
    void showEvent(QShowEvent *e);
    virtual void setupUI(const QStringList &events);
    virtual void updatePixmap(bool isset, int position);

signals:
    void updateAlarm(bool value);
    void updateColor(QColor);
};
