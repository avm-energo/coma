#ifndef ALARM_H
#define ALARM_H

#include "../gen/datatypes.h"
#include "basealarm.h"

class ModuleAlarm : public BaseAlarm
{
    Q_OBJECT
public:
    int m_startAlarmAddress;
    explicit ModuleAlarm(QWidget *parent = nullptr);
    void reqUpdate() override
    {
    }

public slots:
    //    void Update(std::bitset<32> &states);
    void update(const DataTypes::SinglePointWithTimeStruct &sp);
};

#endif // ALARM_H
