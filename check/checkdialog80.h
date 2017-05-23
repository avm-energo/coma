#ifndef E_CHECKDIALOG_H
#define E_CHECKDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QtXlsx/xlsxdocument.h>

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
        float Vbat;                 // 1 напряжение батареи, В
        float Frequency;            // 2 частота сигналов, Гц,
        float IUefNat_filt[6];      // 3-8 Истинные действующие значения сигналов
                                    // в вольтах и амперах,
        float IUeff_filtered[6];    // 9-14 Действующие значения сигналов по 1-й гармонике,
        float phi_next_f[6];        // 15-20 Углы сдвига между сигналами по 1-й гармонике
                                    // в 6 каналах в градусах,
        float PNatf[3];             // 21-23 Истинная активная мощность, по фазам,
        float SNatf[3];             // 24-26 Кажущаяся полная мощность,
                                    // по эффективным  токам и напряжениям,
        float QNatf[3];             // 27-29 Реактивная мощность по кажущейся полной,
                                    // и истинной активной,
        float CosPhiNat[3];         // 30-32 cos phi по истинной активной мощности,
        float Pf[3];                // 33-35 Активная мощность по 1-й гармонике,
        float Qf[3];                // 36-38 Реактивная мощность по 1-й гармонике,
        float Sf[3];                // 39-41 Полная мощность по 1-й гармонике,
        float CosPhi[3];            // 42-44 cos phi по 1-й гармонике
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
