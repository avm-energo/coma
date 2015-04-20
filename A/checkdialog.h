#ifndef A_CHECKDIALOG_H
#define A_CHECKDIALOG_H

#include <QDialog>
#include <QTimer>
#include "canal.h"

class a_checkdialog : public QDialog
{
    Q_OBJECT
public:
    explicit a_checkdialog(QWidget *parent = 0);

signals:

public slots:

private:
    canal *cn;
    struct Bd
    {
        float ain[16];
        float in[16];
        float at;
        float ainn[16];
        float ainx[16];
    };

    struct Bda
    {
        quint32 sin[16];
    };

    Bd Bd_block;
    Bda Bda_block;
    bool BdMeasurementsActive, BdaMeasurementsActive, OddTimeout;
    QTimer *timer;

    void SetupUI();
    void ShowErrMsg(int);

private slots:
    void StartMeasurements();
    void StartBdaMeasurements();
    void StopMeasurements();
    void StopBdaMeasurements();
    void SetTimerPeriod(int);
    void CheckLEDOn();
    void CheckLEDOff();
    void GetIP();
    void Check1PPS();
    void TimerTimeout();
    void RefreshBd();
    void RefreshBda();
};

#endif // A_CHECKDIALOG_H
