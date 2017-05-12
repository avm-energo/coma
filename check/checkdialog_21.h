#ifndef A_CHECKDIALOG_H
#define A_CHECKDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QTime>
#include "../config/config21.h"
#include "../publicclass.h"
#include <QtXlsx/xlsxdocument.h>

#define ACHECKNUMPOINTS  64 // количество точек, по которым выдаются значения в блоке Bda

class a_checkdialog : public QDialog
{
    Q_OBJECT
public:
    explicit a_checkdialog(QWidget *parent = 0);

signals:

public slots:

private:
    struct Bd
    {
        float ain[16];
        float inI[16];
        float inU[16];
        float at;
        float ainn[16];
        float ainx[16];
    };

    struct Bda
    {
        quint32 sin[16];
    };

    struct Bip
    {
        quint8 ip[4];
    };

    Bd Bd_block;
    Bda Bda_block;
    Bip Bip_block;
    Config *aconf;
    bool BdMeasurementsActive, BdaMeasurementsActive, OddTimeout;
    QTimer *timer;
    bool XlsxWriting;
    QXlsx::Document *xlsx;
    int WRow;
    QTime *ElapsedTimeCounter;

    void SetupUI();
    void RefreshBd();
    void RefreshBda();
    void CheckIP();

private slots:
    void StartMeasurements();
    void StartMeasurementsWithFile();
    void StartBdaMeasurements();
    void StopMeasurements();
    void StopBdaMeasurements();
    void SetTimerPeriod();
    void CheckLEDOn();
    void CheckLEDOff();
    void GetIP();
    void Check1PPS();
    void TimerTimeout();
};

#endif // A_CHECKDIALOG_H
