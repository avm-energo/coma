#ifndef ALARM_H
#define ALARM_H

#include <QBitArray>
#include <QWidget>

class Alarm : public QWidget
{
    Q_OBJECT
public:
    explicit Alarm(QWidget *parent = nullptr);

public slots:
    void Update(QList<bool> states);

private:
    int m_alarmBdNum;
    int m_startAlarmAddress;
    QBitArray m_alarmFlags; // '1' equals alarm

protected:
    void showEvent(QShowEvent *e);
    void UpdatePixmaps(quint32 alarm, int counter, bool warn = false);
    void SetupAlarm(const QStringList &events, int counters);
};

#endif // ALARM_H
