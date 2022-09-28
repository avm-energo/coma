#pragma once

#include "../module/basealarm.h"

constexpr quint32 BSIWARNMASK = 0x00005F55;  ///< Warning alarms bit mask.
constexpr quint32 BSIALARMMASK = 0x000020AA; ///< Critical alarms bit mask.

/// \brief The class for alarm dialog, that displaying the device general status.
class AlarmStateAll : public BaseAlarm
{
    Q_OBJECT
private:
    std::bitset<128> mAlarmFlags; // '1' equals alarm

public:
    explicit AlarmStateAll(QWidget *parent = nullptr);
    virtual void setupUI(const QStringList &events) override;
    virtual void reqUpdate() override;

public slots:
    void update(quint32 health);
};
