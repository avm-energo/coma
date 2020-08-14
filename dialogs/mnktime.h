#ifndef MNKTIME_H
#define MNKTIME_H

#include "../iec104/iec104.h"
#include "../modbus/modbus.h"

#include <QByteArray>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QTime>

class MNKTime : public QDialog
{
    Q_OBJECT

public:
    explicit MNKTime(QWidget *parent = nullptr);
    ~MNKTime();

    bool First;
    QTimer *Timer;

private:
    void SetupUI();
    void SetTime(quint32 unixtimestamp);
    void WriteTime(QDateTime &myDateTime);

signals:
    void ethTimeRequest();
    void modBusTimeRequest();
    void ethWriteTimeToModule(uint);
    void modbusWriteTimeToModule(uint);

public slots:
    void ErrorRead();
    void TimeWritten();

private slots:
    void slot_timeOut();
    void slot2_timeOut();
    void Write_PCDate();
    void Write_Date();
    void FillTimeFrom104(IEC104Thread::BS104Signals *);
    void FillTimeFromModBus(QList<ModBus::BSISignalStruct>);
};

#endif // MNKTIME_H
