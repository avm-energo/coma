#ifndef TIME_H
#define TIME_H

#include <QDialog>
#include <QByteArray>
#include <QLabel>
#include <QTime>
#include <QLineEdit>

#include "../widgets/etablemodel.h"

class MNKTime : public QDialog
{
    Q_OBJECT

public:
    explicit MNKTime(QWidget *parent = nullptr);
    ~MNKTime();

    int timeIndex;
    QLabel *SysTime;
    QLineEdit *SysTime2;
    QTimer *timer;
    QTimer *timerRead;
    //QThread *thr;
    bool FinishThread, closeThr, haveFinished;
    uint32_t unixtimestamp = 1423062000;
    QDateTime myDateTime;

private:


    void SetupUI();

signals:
    void finished();


public slots:


private slots:
    void slot_timeOut();
    void slot2_timeOut();
    void Start_Timer(int);
    void Stop_Timer(int);
    void Write_PCDate();
    void Write_Date();
    void StopSlot();
//#if PROGSIZE != PROGSIZE_EMUL

//#endif
};

#endif // TIME_H
