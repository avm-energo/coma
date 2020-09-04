#ifndef TuneDialogKDV_H
#define TuneDialogKDV_H

#include <QDialog>
#include "eabstracttunedialog.h"
#include "../check/checkKDV.h"
#include "../gen/modulebsi.h"
#include "../config/configKDV.h"
#include "../iec104/iec104.h"
#include <QStandardItemModel>
#include "limereport/lrreportengine.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include "eabstracttunedialog.h"
#include "../config/config.h"

#define TUNEFILELENGTH  256
#define CKDV_BDA_IN 1

// voltages
#define V60     60.0
#define V57     57.74
// frequencies
#define HZ50    50.0
// currents
#define I_1      1.0
#define I_5      5.0

// parameters for GetExtData
#define TD_GED_U    0x01 // напряжение
#define TD_GED_I    0x02 // ток
#define TD_GED_F    0x04 // частота
#define TD_GED_D    0x08 // угол нагрузки

#define TD_TMK  25.0 // degrees
#define TD_VBAT 3.0 // voltage
#define TD_FREQ 50 // Hz
#define CONST2PIF 314.15926

class QGroupBox;

class TuneDialogKDV : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialogKDV(QVector<S2::DataRec> *S2Config, QWidget *parent = nullptr);
    //~TuneDialog85();
    int inc;
    float samples[6];

signals:
    void Send();

public slots:

private:

    ConfigKDV *CKDV;
    QVector<S2::DataRec> *S2Config;
    ConfigKDV::Bci Bci_block_work;
    QDialog *ask;
    QLineEdit *ledit;
    QTimer *timer;


    enum TuneModes
    {
        TUNEMIP,
        TUNERET,
        TUNEMAN
    };
    enum PovTypes
    {
        GOST_NONE, // не задано
        GOST_23625, // по 5 точкам туда-сюда
        GOST_1983 // по 3 точкам только туда
    };

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

    bool Cancelled = false, DefConfig;
    //QVector<S2::DataRec> *S2Config;
    //QVector<S2::DataRec> *S2ConfigForTune;  // для регулировки
    IEC104 *mipcanal;
    int TuneControlType;
    int SecondsToEnd15SecondsInterval;
    //QHash <QString, int (TuneDialog80::*)()> pf;
    LimeReport::ReportEngine *report;


    void SetupUI();

    struct Bac1 {	// Структура калибровочных параметров
        //записываются во флеш базовой платы
    float K_freq;		// коррекция частоты
    float Art;		// коэффициент в канале Pt100, ед.АЦП/Ом
    float Brt;		// смещение в канале Pt100, ед.АЦП
    };

    struct Bac2 {	// Структура калибровочных параметров
        //записываются во флеш мезонинной платы:
    //quint32 N1_TT[3];	// Число витков первичной обмотки
    float KmU1[3];		// калибровочные коэффициенты по напряжению в 3 каналах для Кацп=1
    float KmU2[3];		// калибровочные коэффициенты по напряжению в 3 каналах для Кацп=2
    float KmU4[3];		// калибровочные коэффициенты по напряжению в 3 каналах для Кацп=4
    float KmI1[3];		// калибровочные коэффициенты по току в 3 каналах для Кацп=1
    float KmI2[3];		// калибровочные коэффициенты по току в 3 каналах для Кацп=2
    float KmI4[3];		// калибровочные коэффициенты по току в 3 каналах для Кацп=4
    float KmI8[3];		// калибровочные коэффициенты по току в 3 каналах для Кацп=8
    float KmI16[3];	// калибровочные коэффициенты по току в 6 каналах для Кацп=16
    float KmI32[3];	// калибровочные коэффициенты по току в 6 каналах для Кацп=32
    float DPsi[6];		// коррекция фазы в i-м канале (в градусах)
    /*float Tmk0;		// Начальная температура МК для коррекции
    float TKUa[6];	//температурные коэффициенты линейной коррекции
    // по напряжениям и токам
    float TKUb[6];	//температурные коэффициенты квадратичной коррекции
    float TKPsi_a[3]; 	//температурные коэффициенты линейной коррекции по tg delta
    float TKPsi_b[3]; 	//температурные коэффициенты квадратичной коррекции по tg delta*/
    };

    Bac1 Bac_block1, Bac_newblock1;
    Bac2 Bac_block2, Bac_newblock2;

    typedef struct {		// Структура блока выходных данных
    // в масштабах входных сигналов (для настройки)
    float Frequency;	// Частота в сети, Гц
    float IUefNat_filt[6];	// Истинные действующие значения сигналов (в вольтах или мА на входе)
    float IUeff_filtered[6];  // действующие значения сигналов по 1-й гармонике
    float phi_next_f[6];	// Углы сдвига сигналов по 1-й гармонике относительно Ua в градусах
    float Cbush[3];	// емкости вводов
    float Tg_d[3];		// tg delta вводов
    float Pt100_R;		// Измеренное сопротивление термометра, Ом
    }Bda_in_struct;

    Bda_in_struct Bda_in, Bda_block20, Bda_block60, Bda_blockMinus20;

    float Uet[3], Iet[3], PHIet[6], FREQet, UetMinus20[3], IetMinus20[3], PHIetMinus20[6], FREQetMinus20, Uet60[3], Iet60[3], PHIet60[6], FREQet60;

    struct BdaStruct
    {
        float Ueff_ADC[6];
        float Frequency;
        float Pt100;
     };

    BdaStruct Bda_block, BdaPt100_80Om, BdaPt100_120Om;

    quint8 TempCor;

    struct RealDataStruct
    {
        float f[3]; // frequencies
        float u[3]; // voltages
        float i[3]; // currents
        float d[3]; // load phase
        float dpsiU[2];//interphase voltages angles (B-A, C-B)
    };

    struct Bd0
    {
        float Tmk;	// Температура кристалла микроконтроллера, °С
        float Vbat;	// Напряжение аккумуляторной батареи, В
    };

    Bd0 Bd_block20, Bd_block_minus20, Bd_block_plus60;

    struct PovDevStruct // данные об установке
    {
        QString DevName; // наименование установки
        QString DevSN; // серийный (заводской) номер
        QString DevPrecision; // точность
    };

    PovDevStruct PovDev;

    struct ReportHeaderStructure
    {
        QString Organization;   // организация, проводившая проверку
        QString Day;            // день месяца проведения проверки
        QString Month;          // месяц
        QString Yr;             // две последние цифры года (20хх)
        QString Freq;           // обозначение частоты
        QString UA;             // напряжение фазы А
        QString UB;             // напряжение фазы B
        QString UC;             // напряжение фазы С
        QString IA;             // ток фазы А
        QString IB;             // ток фазы B
        QString IC;             // ток фазы C
        QString PhiloadA;       // угол нагрузки фазы А
        QString PhiloadB;       // угол нагрузки фазы B
        QString PhiloadC;       // угол нагрузки фазы C
        QString PhiUAB;         // угол между напряжениями фаз А и B
        QString PhiUBC;         // угол между напряжениями фаз B и C
        QString OffsetF;        // Погрешности
        QString OffsetUA;
        QString OffsetUB;
        QString OffsetUC;
        QString OffsetIA;
        QString OffsetIB;
        QString OffsetIC;
        QString OffsetPhiloadA;
        QString OffsetPhiloadB;
        QString OffsetPhiloadC;
        QString OffsetPhiUAB;
        QString OffsetPhiUBC;
    };

    QStringList TableItem;      // строка таблицы с данными вывода
    QList<QStringList *> MainData; // полные данные таблицы для модели

    ReportHeaderStructure ReportHeader;
    int N;

    int Index, Counter;
    float CurrentS; // текущее значение нагрузки
    int PovType, TempPovType; // тип поверяемого оборудования (по какому ГОСТу)
    QStandardItemModel *ReportModel, *ViewModel; // модель, в которую заносим данные для отчёта
    int RowCount, ColumnCount; // количество рядов и столбцов в модели
    bool Autonomous; // =1, если производится формирование протокола из файла, =0 - при работе с прибором
    bool TempFromLE, HumFromLE; // =1, если данные в протокол надо брать из поля ввода, =0 - если из прибора

    double U[21] =       {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 72, 48, 36, 24, 12, 60, 60, 60, 60 };
    double I[21] =       {0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1, 2, 3, 4, 5, 6,  5, 5, 5, 5, 5, 5, 5, 5, 5 };
    double PhiLoad[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 90, 180, 270 };

    RealDataStruct RealData;
    float IUefNat_filt_old[6];      // для сохранения значений по п. 7.3.2
    float MipDat[46];
    int GED_Type;
    int Kadc;

    QHBoxLayout *MipPars(int parnum, const QString &groupname);
    void FillBac(int bacnum);
    void FillBackBac(int bacnum);
    void PrepareConsts();
    void FillNewBac();


#if PROGSIZE != PROGSIZE_EMUL
    void SetLbls();
    void SetPf();
    void Tune3p();
    int CheckTuneCoefs();
    int CheckMip();
    bool IsWithinLimits(double number, double base, double threshold);
    int ShowControlChooseDialog();
    int Show3PhaseScheme();
    void Show1RetomDialog(float U, float A);
    void GetBdAndFillMTT();
    int Start7_2_3();
    int Start7_3_1();
    int Start7_3_1_1();
    int Start7_3_2();
    int Start7_3_4();
    int Start7_3_4_2();
    int Start7_3_4_6();
    int Start7_3_4_7();
    int Start7_3_4_11();
    int Start7_3_4_12();
    int Start7_3_4_13();
    int Start7_3_4_14();
    int Start7_3_4_15();
    int Start7_3_4_16();
    int Start7_3_4_17();
    int Start7_3_4_18();
    int Start7_3_5();
    int SaveUeff();
    int ShowRetomDialog(double U, double I, double Y);
    int GetExternalData(); // ввод данных в зависимости от выбранного режима и номера опыта
    int SetNewTuneCoefs(); // заполнение Bac_newblock, чтобы не было пурги после настройки
    int SaveWorkConfig();
    int LoadWorkConfig();
    QWidget *Bd1W(QWidget *parent);
    void FillBd1(QWidget *parent);
    void RefreshAnalogValues(int bdnum);
    int Show80();
    int Show120();
    void CalcNewPt100Coefs();
    void CalcTempCorCoefs();
    void EnterDataTune();
    int Show125mA();
    int AskTemp20();
    int AskTempMinus20();
    int AskTemp60();
    int Average();
    int ReadAnalogTemp20();
    int ReadAnalogTempMinus20();
    int ReadAnalogTemp60();
    void Enter20Data();
    void Enterminus20Data();
    void Enter60Data();

#endif

    float ToFloat(QString text);

    QString ValuesFormat, WidgetFormat;


private slots:

#if PROGSIZE != PROGSIZE_EMUL
    int TuneChannel();
    void ReadN();
    void StartMip();
    void StopMip();
    void ParseMipData(Parse104::FlSignals104 &);
    void SetTuneMode();
    int ReadAnalogMeasurements();
    void SetExtData();
    void CancelExtData();
    void CancelTune();
    void GenerateReport();
    int TunePt100Channel();
    int TuneTemp();
    int TuneTempCor();
    int CalcTuneCoefs();
    void SaveValuesTemp20();
    void SaveValuesTempMinus20();
    void SaveValuesTemp60();
    void CloseAsk();


#endif
    void SetDefCoefs();

};


#endif // TUNEDIALOG85_H
