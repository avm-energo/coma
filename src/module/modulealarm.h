#ifndef ALARM_H
#define ALARM_H

#include "../gen/datatypes.h"
#include "basealarm.h"

class ModuleAlarm : public BaseAlarm
{
    Q_OBJECT
public:
    explicit ModuleAlarm(QWidget *parent = nullptr);
    ModuleAlarm(const DataTypes::Alarm &desc, const int count, QWidget *parent = nullptr);
    void reqUpdate() override;

protected:
    /// Стартовый адрес регистров сигнализации
    quint32 m_startAlarmAddress;
    /// Общее количество регистров сигнализации
    quint32 m_alarmAllCounts;
public slots:
    //    void Update(std::bitset<32> &states);
    void update(const QVariant &sp);
    void update();
};

#endif // ALARM_H
