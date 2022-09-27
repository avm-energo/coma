#ifndef ALARMSTATEALL_H
#define ALARMSTATEALL_H

#include "../module/basealarm.h"

constexpr quint32 BSI_HEALTH_MASK = 0x0000FFFF;
constexpr quint32 BSIWARNMASK = 0x00005F55;
constexpr quint32 BSIALARMMASK = 0x000020AA;

class AlarmStateAll : public BaseAlarm
{
    Q_OBJECT
private:
    std::bitset<128> mAlarmFlags; // '1' equals alarm

public:
    explicit AlarmStateAll(QWidget *parent = nullptr);
    virtual void setupUI(const QStringList &events) override;
    virtual void reqUpdate() override;
    void update(quint32 health);
};

#endif // ALARMSTATE_H
