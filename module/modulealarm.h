#ifndef ALARM_H
#define ALARM_H

#include "../gen/datatypes.h"
#include "basealarm.h"

class ModuleAlarm : public BaseAlarm
{
    Q_OBJECT
public:
    /// Стартовый адрес регистров сигнализации
    int m_startAlarmAddress;
    /// Общее количество регистров сигнализации
    int m_alarmAllCounts;
    explicit ModuleAlarm(QWidget *parent = nullptr);
    void reqUpdate() override;

public slots:
    //    void Update(std::bitset<32> &states);
    void update(const DataTypes::SinglePointWithTimeStruct &sp);
};

#endif // ALARM_H
