#ifndef ALARM_H
#define ALARM_H

#include <QBitArray>
#include <QWidget>
#include <bitset>

#define CIRCLE_RADIUS 15

class Alarm : public QWidget
{
    Q_OBJECT
public:
    explicit Alarm(QWidget *parent = nullptr);
    int m_alarmBdNum;
    int m_startAlarmAddress;
    std::bitset m_alarmFlags; // '1' equals alarm

public slots:
    void Update(QList<bool> states);

private:
protected:
    void showEvent(QShowEvent *e);
    void UpdatePixmaps(bool isset, int position, bool warn = false);
    void SetupUI(const QStringList &events, int counters);
};

#endif // ALARM_H
