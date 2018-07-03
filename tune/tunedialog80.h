#ifndef TUNEDIALOG80_H
#define TUNEDIALOG80_H

#include <QCloseEvent>
#include <QHBoxLayout>
#include <QByteArray>
#include <QStringList>
#include "../config/config80.h"
#include "../iec104/iec104.h"
#include "eabstracttunedialog.h"

#define TUNEFILELENGTH  256

// voltages
#define V60     60.0
#define V57     57.74
// frequencies
#define HZ50    50.0
// currents
#define I1      1.0
#define I5      5.0

// parameters for GetExtData
#define TD_GED_U    0x01 // напряжение
#define TD_GED_I    0x02 // ток
#define TD_GED_F    0x04 // частота
#define TD_GED_D    0x08 // угол нагрузки

#define TD_TMK  25.0 // degrees
#define TD_VBAT 3.0 // voltage
#define TD_FREQ 50 // Hz

class TuneDialog80 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialog80(QVector<S2::DataRec> &S2Config, QWidget *parent = 0);

signals:
    void stopall();
    void SendMip(QByteArray);
    void dataready(QByteArray);
    void SecondsRemaining(QString);

public slots:

private:
    enum TuneModes
    {
        TUNEMIP,
        TUNERET,
        TUNEMAN
    };

    bool Cancelled, DefConfig;
    Config80 *C80;
    QVector<S2::DataRec> *S2Config;
    Config80::Bci Bci_block_work;
    iec104 *mipcanal;
    int TuneControlType;
    int SecondsToEnd15SecondsInterval;
    QHash <QString, int (TuneDialog80::*)()> pf;

    struct Bac
    {
        float KmU[6];
        float KmI_5[6];
        float KmI_1[6];
        float DPsi[6];
        float K_freq;
        float Kinter;
    };

    Bac Bac_block, Bac_newblock;

    struct MipValues
    {
        double u;
        double i[3];
    };

    MipValues MVTC;

    struct MipTolerances
    {
        double u;
        double i;
    };

    MipTolerances MTTC;

    struct BdaValues
    {
        double tmk;
        double bat;
        double freq;
        double v1;
        double v2;
        double phi;
        double p;
        double s;
        double q;
        double cosphi;
    };

    BdaValues VTCG;

    struct BdaTolerances
    {
        double v1;
        double v2;
        double phi;
        double p;
        double cosphi;
    };

    BdaTolerances TTCG;

    struct RealDataStruct
    {
        float f[3]; // frequencies
        float u[3]; // voltages
        float i[3]; // currents
        float d[3]; // load phase
    };

    RealDataStruct RealData;
    float IUefNat_filt_old[6];      // для сохранения значений по п. 7.3.2
    float MipDat[41];
    int GED_Type;

    void SetupUI();
    QHBoxLayout *MipPars(int parnum, const QString &groupname);
    void FillBac();
    void FillBackBac();
    void PrepareConsts();
#if PROGSIZE != PROGSIZE_EMUL
    void SetLbls();
    void SetPf();
    void GetBdAndFillMTT();
    void Tune3p();
    int CheckTuneCoefs();
    int CheckMip();
    bool IsWithinLimits(double number, double base, double threshold);
    int ShowControlChooseDialog();
    int Show3PhaseScheme();
    void Show1RetomDialog(float U, float A);
    int Start7_2_3();
    int Start7_3_1();
    int Start7_3_1_1();
    int Start7_3_2();
    int Start7_3_3();
    int Start7_3_4();
    int Start7_3_5();
    int Start7_3_6_2();
    int Start7_3_7_1();
    int Start7_3_7_2();
    int Start7_3_7_3();
    int Start7_3_7_4();
    int Start7_3_7_5();
    int Start7_3_7_6();
    int Start7_3_7_7();
    int Start7_3_7_8();
    int Start7_3_7_10();
    int Start7_3_8_1();
    int Start7_3_8_2();
    int Start7_3_9();
    int SaveUeff();
    int ShowRetomDialog(double U, double I);
    int TuneDialog80::StartCheckAnalogValues(double u, double i, double deg, bool tol); // deg - угол в градусах между токами и напряжениями одной фазы, tol - 0: начальная точность, 1 - повышенная
    int CheckAnalogValues(double u, double i, double p, double q, double s, double phi, double cosphi, double utol, double itol, double pht, double pt, double ct);
    bool SetConfA(int i2nom);
    int GetExternalData(); // ввод данных в зависимости от выбранного режима и номера опыта
    void MsgClear();
    int SetNewTuneCoefs(); // заполнение Bac_newblock, чтобы не было пурги после настройки
    int SaveWorkConfig();
    int LoadWorkConfig();
#endif
    float ToFloat(QString text);
private slots:
#if PROGSIZE != PROGSIZE_EMUL
    void StartMip();
    void StopMip();
    void ParseMipData(Parse104::Signals104 &);
    void SetTuneMode();
    int ReadAnalogMeasurements();
    void SetExtData();
    void CancelExtData();
    void CancelTune();
#endif
    void SetDefCoefs();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // TuneDialog80_H
