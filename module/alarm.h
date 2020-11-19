#ifndef ALARM_H
#define ALARM_H

#include "../gen/datatypes.h"

#include <QBitArray>
#include <QWidget>
#include <bitset>

#define CIRCLE_RADIUS 15

class Alarm : public QWidget
{
    Q_OBJECT
public:
    int m_alarmBdNum;
    int m_startAlarmAddress;
    std::bitset<32> m_alarmFlags;       // '1' equals alarm
    std::bitset<32> m_actualAlarmFlags; // '1' equals alarm

    explicit Alarm(QWidget *parent = nullptr);
    int realAlarmSize();
    void updatePixmap(bool isset, int position);

public slots:
    //    void Update(std::bitset<32> &states);
    void updateSpData(const DataTypes::SinglePointWithTimeStruct &sp);

signals:
    void updateAlarm(bool value);

private:
    int m_realAlarmSize;

protected:
    void showEvent(QShowEvent *e);
    void setupUI(const QStringList &events);
};

#endif // ALARM_H
