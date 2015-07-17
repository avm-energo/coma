#ifndef E_TUNEDIALOG_H
#define E_TUNEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QByteArray>
#include "../iec104/iec104.h"

#define TUNEFILELENGTH  256

#define TUNEMIP 1
#define TUNERET 2
#define TUNEMAN 3

class e_tunedialog : public QDialog
{
    Q_OBJECT
public:
    explicit e_tunedialog(QWidget *parent = 0);

signals:
    void stopall();
    void SendMip(QByteArray);
    void dataready(QByteArray);
    void error(int);

public slots:

private:
    iec104 *mipcanal;
    int TuneControlType;
    bool MipErrNeeded;

    struct Bac
    {
        float KmU[6];
        float KmI_5[6];
        float KmI_1[6];
        float DPsi[6];
        float K_freq;
        float Kinter;
    };

    Bac Bac_block, Bac_defblock;

    struct Bda
    {
        float Tmk;                  // Температура кристалла микроконтроллера,
        float Frequency;            // частота сигналов, Гц,
        float IUefNat_filt[6];      // Истинные действующие значения сигналов
                                    // в вольтах и амперах,
        float IUeff_filtered[6];    // Действующие значения сигналов по 1-й гармонике,
        float phi_next_f[6];        // Углы сдвига между сигналами по 1-й гармонике
                                    // в 6 каналах в градусах,
        float PNatf[3];             // Истинная активная мощность, по фазам,
        float SNatf[3];             // Кажущаяся полная мощность,
                                    // по эффективным  токам и напряжениям,
        float QNatf[3];             // Реактивная мощность по кажущейся полной,
                                    // и истинной активной,
        float CosPhiNat[3];         // cos phi по истинной активной мощности,
        float Pf[3];                // Активная мощность по 1-й гармонике,
        float Qf[3];                // Реактивная мощность по 1-й гармонике,
        float Sf[3];                // Полная мощность по 1-й гармонике,
        float CosPhi[3];            // cos phi по 1-й гармонике
    };

    Bda Bda_block;

    void SetupUI();
    void CalcNewTuneCoefs();
    void RefreshTuneCoefs();
    bool CheckTuneCoefs();
    bool CheckAnalogValues(int ntest);
    bool CheckMip();
    bool IsWithinLimits(double number, double base, double threshold);
    void RefreshAnalogValues();

private slots:
    void StartTune();
    void ReadTuneCoefs();
    void WriteTuneCoefs();
    void SaveToFile();
    void LoadFromFile();
    void StartMip();
    void StopMip();
    void DeleteMip();
    void EthConnected();
    void EthDisconnected();
    void MipData(Parse104::Signals104 &);
    void SetTuneMip();
    void SetTuneRetom();
    void SetTuneManual();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // E_TUNEDIALOG_H
