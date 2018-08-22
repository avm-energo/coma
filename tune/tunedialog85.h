#ifndef TUNEDIALOG85_H
#define TUNEDIALOG85_H

#include <QDialog>
#include "eabstracttunedialog.h"
#include "../check/check85.h"
#include "../gen/modulebsi.h"
#include "../config/config85.h"
#include "../iec104/iec104.h"
#include <QHBoxLayout>

#define TUNEFILELENGTH  256

// voltages
#define V60     60.0
#define V57     57.74
// frequencies
#define HZ50    50.0
// currents
#define I1      1.0
#define I4      4.0

// parameters for GetExtData
#define TD_GED_U    0x01 // напряжение
#define TD_GED_I    0x02 // ток
#define TD_GED_F    0x04 // частота
#define TD_GED_D    0x08 // угол нагрузки

#define TD_TMK  25.0 // degrees
#define TD_VBAT 3.0 // voltage
#define TD_FREQ 50 // Hz

class QGroupBox;

class TuneDialog85 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
   // explicit TuneDialog85(BoardTypes type = BoardTypes::BT_BASE, QWidget *parent = nullptr);
    explicit TuneDialog85(QVector<S2::DataRec> &S2Config, QWidget *parent = nullptr);
    //~TuneDialog85();

signals:

public slots:

private:
    enum TuneModes
    {
        TUNEMIP,
        TUNERET,
        TUNEMAN
    };
    int GED_Type;
    float IUefNat_filt_old[6];      // для сохранения значений по п. 7.3.2
    float MipDat[41];

    //bool Cancelled, DefConfig;
    Config85 *C85;
    QVector<S2::DataRec> *S2Config;
    Config85::Bci Bci_block_work;
    int TuneControlType;
    void SetupUI();
    QHBoxLayout *MipPars(int parnum, const QString &groupname);
    void FillBac();
    void FillBackBac();

    struct Bac
    {
        float KmU[3];
        float KmI_1[3];
        float KmI_4[3];
        float K_freq;
        float KmU2[3];
    };

    Bac Bac_block, New_Bac_block;

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

    struct Bda_in_struct{
        float Frequency;            // Частота в сети
        float IUefNat_filt[9];  	// Истинные действующие значения сигналов (в вольтах или амперах на входе)
        float UefNatLin_filt[6];	// Истинные действующие значения линейных напряжений 1-й и 2-й групп
        float PNatf[3];             // истинная активная мощность по фазам
        float QNatf[3];             // реактивная мощность по кажущейся полной и истинной активной
        float SNatf[3];             // кажущаяся полная мощность по эфф. токам и напряжениям
        float CosPhiNat[3];         // cos phi по истинной активной мощности, по фазам
        quint32 DD_in;


    };

    Bda_in_struct Bda_Block;
    iec104 *mipcanal;

    float ToFloat(QString text);
    void CancelTune();
    void closeEvent(QCloseEvent *e);
    int SaveWorkConfig();
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
    //int TuneDialog85::StartCheckAnalogValues(double u, double i, double deg, bool tol); // deg - угол в градусах между токами и напряжениями одной фазы, tol - 0: начальная точность, 1 - повышенная
    int CheckAnalogValues(double u, double i, double p, double q, double s, double phi, double cosphi, double utol, double itol, double pht, double pt, double ct);
    bool SetConfA(int i2nom);
    int GetExternalData(); // ввод данных в зависимости от выбранного режима и номера опыта
    void MsgClear();
    int SetNewTuneCoefs(); // заполнение Bac_newblock, чтобы не было пурги после настройки
    int LoadWorkConfig();
    int StartCheckAnalogValues(double u, double i, double deg, bool tol); // deg - угол в градусах между токами и напряжениями одной фазы, tol - 0: начальная точность, 1 - повышенная


private slots:
#if PROGSIZE != PROGSIZE_EMUL
    void StartMip();
    void StopMip();
    void ParseMipData(Parse104::Signals104 &);
    void SetTuneMode();
    int ReadAnalogMeasurements();
    void SetExtData();
    void CancelExtData();
    int TuneOneChannel(int Ch);
#endif

    void SetDefCoefs();

};


#endif // TUNEDIALOG85_H
