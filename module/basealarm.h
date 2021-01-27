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
    //    explicit BaseAlarm(QWidget *parent = nullptr);

    //    void setInterface(BaseInterface *interface) noexcept;
    //    BaseInterface *iface() const noexcept;
    void disable();
public slots:
    virtual void reqUpdate() = 0;

protected:
    std::bitset<32> m_alarmFlags;       // '1' equals alarm
    std::bitset<32> m_actualAlarmFlags; // '1' equals alarm

    QColor m_normalColor;
    QColor m_alarmColor;
    void showEvent(QShowEvent *e);
    virtual void setupUI(const QStringList &events);
    virtual void updatePixmap(bool isset, int position);

    //    BaseInterface *m_iface;
signals:
    void updateAlarm(bool value);
    void updateColor(QColor);
};
