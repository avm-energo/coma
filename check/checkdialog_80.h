#ifndef E_CHECKDIALOG_H
#define E_CHECKDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QtXlsx/xlsxdocument.h>

#define ECHECKER(a)       ERMSG(publicclass::ER_ECHECK,__LINE__,a)
#define ECHECKINFO(a)     INFOMSG(publicclass::ER_ECHECK,__LINE__,a)

class e_checkdialog : public QDialog
{
    Q_OBJECT
public:
    explicit e_checkdialog(QWidget *parent = 0);

signals:

public slots:

private:

    struct Bda
    {
        float Tmk;                  // 0 Температура кристалла микроконтроллера,
        float Frequency;            // 1 частота сигналов, Гц,
        float IUefNat_filt[6];      // 2-7 Истинные действующие значения сигналов
                                    // в вольтах и амперах,
        float IUeff_filtered[6];    // 8-13 Действующие значения сигналов по 1-й гармонике,
        float phi_next_f[6];        // 14-19 Углы сдвига между сигналами по 1-й гармонике
                                    // в 6 каналах в градусах,
        float PNatf[3];             // 20-22 Истинная активная мощность, по фазам,
        float SNatf[3];             // 23-25 Кажущаяся полная мощность,
                                    // по эффективным  токам и напряжениям,
        float QNatf[3];             // 26-28 Реактивная мощность по кажущейся полной,
                                    // и истинной активной,
        float CosPhiNat[3];         // 29-31 cos phi по истинной активной мощности,
        float Pf[3];                // 32-34 Активная мощность по 1-й гармонике,
        float Qf[3];                // 35-37 Реактивная мощность по 1-й гармонике,
        float Sf[3];                // 38-40 Полная мощность по 1-й гармонике,
        float CosPhi[3];            // 41-43 cos phi по 1-й гармонике
    };

    struct Bip
    {
        quint8 ip[4];
    };

    Bda Bda_block;
    Bip Bip_block;
    QTimer *timer;
    QXlsx::Document *xlsx;
    bool XlsxWriting;
    int WRow;
    QTime *ElapsedTimeCounter;

    void SetupUI();
    void RefreshAnalogValues();

private slots:
    void StartAnalogMeasurementsToFile();
    void StartAnalogMeasurements();
    void StopAnalogMeasurements();
    void ReadAnalogMeasurements();
    void GetIP();
    void CheckIP();
    void Check1PPS();
    void SetTimerPeriod();
};

#endif // E_CHECKDIALOG_H
