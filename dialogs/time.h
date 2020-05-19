#ifndef TIME_H
#define TIME_H

#include <QDialog>
#include <QByteArray>
#include <QLabel>
#include <QTime>
#include <QLineEdit>
#include "../iec104/iec104.h"
#include "../modBus/modBus.h"

#include "../widgets/etablemodel.h"

class MNKTime : public QDialog
{
    Q_OBJECT

public:
    explicit MNKTime(QWidget *parent = nullptr);
    ~MNKTime();

    typedef struct
    {
       quint32 Val;
       int SigAdr;
    }ModBusBSISignal;

    int timeIndex,checkIndex;
    QLabel *SysTime;
    QLineEdit *SysTime2;
    QTimer *timer;
    QTimer *timerRead;
    //QThread *thr;
    bool FinishThread, closeThr, haveFinished;
    uint32_t unixtimestamp = 1423062000;
    QDateTime myDateTime;
    bool first;

private:


    void SetupUI();

signals:
    void finished();
    void ethTimeRequest();
    void modBusTimeRequest();
    void ethWriteTimeToModule(uint*);
    void modbusWriteTimeToModule(uint*);


public slots:


private slots:
    void slot_timeOut();
    void slot2_timeOut();
    void Start_Timer(int);
    void Stop_Timer(int);
    void Write_PCDate();
    void Write_Date();
    void StopSlot();
    void FillTimeFrom104(Parse104::BS104Signals*);
    void FillTimeFromModBus(ModBusBSISignal*);
    void ErrorRead();
//#if PROGSIZE != PROGSIZE_EMUL

//#endif
};

#endif // TIME_H
