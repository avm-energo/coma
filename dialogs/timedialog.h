#ifndef MNKTIME_H
#define MNKTIME_H

#include "../gen/udialog.h"
#include "../iec104/iec104.h"
#include "../modbus/modbus.h"

class TimeDialog : public UDialog
{
    Q_OBJECT

public:
    explicit TimeDialog(QWidget *parent = nullptr);
    ~TimeDialog();

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
    void slot_timeOut();
    void USBUpdate();
    void MBSUpdate();
    void ETHUpdate();
    void update();

private slots:

    void Write_PCDate();
    void Write_Date();
};

#endif // MNKTIME_H
