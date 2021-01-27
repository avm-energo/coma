#ifndef MNKTIME_H
#define MNKTIME_H

#include "../widgets/udialog.h"

class TimeDialog : public UDialog
{
    Q_OBJECT

public:
    explicit TimeDialog(QWidget *parent = nullptr);
    ~TimeDialog();

    bool First;
    QTimer *Timer;

    int timeZone() const;
    void setTimeZone(int timeZone);

private:
    void SetupUI();
    void SetTime(quint32 unixtimestamp);
    void WriteTime(QDateTime &myDateTime);

    int m_timeZone;

signals:
    //    void ethTimeRequest();
    //    void modBusTimeRequest();
    //    void ethWriteTimeToModule(uint);
    //    void modbusWriteTimeToModule(uint);

public slots:
    void ErrorRead();
    void TimeWritten();
    // void updateSysTime();
    //    void USBUpdate();
    //    void MBSUpdate();
    //    void ETHUpdate();
    void reqUpdate();

private slots:

    void Write_PCDate();
    void Write_Date();
    void uponInterfaceSetting();
    void updateBitStringData(const DataTypes::BitStringStruct &bs);
};

#endif // MNKTIME_H
