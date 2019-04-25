#ifndef TUNEDIALOG84_H
#define TUNEDIALOG84_H

#include <QDialog>
#include "eabstracttunedialog.h"
#include "../check/check84.h"
#include "../gen/modulebsi.h"
#include "../config/config84.h"
#include "../iec104/iec104.h"
#include <QStandardItemModel>
#include "limereport/lrreportengine.h"
#include <QHBoxLayout>
#include "eabstracttunedialog.h"
#include "../config/config.h"

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

class QGroupBox;

class TuneDialog84 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialog84(QVector<S2::DataRec> &S2Config, QWidget *parent = nullptr);
    //~TuneDialog85();

signals:

public slots:

private:

    Config84 *C84;
    QVector<S2::DataRec> *S2Config;
    Config84::Bci Bci_block_work;

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
    iec104 *mipcanal;
    int TuneControlType;
    int SecondsToEnd15SecondsInterval;
    //QHash <QString, int (TuneDialog80::*)()> pf;
    LimeReport::ReportEngine *report;


    void SetupUI();

    struct Bac {	// Структура калибровочных параметров
        //записываются во флеш мезонинной платы:
    quint32 N1_TT[3];	// Число витков первичной обмотки
    float KmU[3];		// калибровочные коэффициенты по напряжению в 6 каналах
    float KmI1[3];		// калибровочные коэффициенты по току в 6 каналах для Кацп=1
    float KmI2[3];		// калибровочные коэффициенты по току в 6 каналах для Кацп=2
    float KmI4[3];		// калибровочные коэффициенты по току в 6 каналах для Кацп=4
    float KmI8[3];		// калибровочные коэффициенты по току в 6 каналах для Кацп=8
    float KmI16[3];	// калибровочные коэффициенты по току в 6 каналах для Кацп=16
    float KmI32[3];	// калибровочные коэффициенты по току в 6 каналах для Кацп=32
    float DPsi[6];		// коррекция фазы в i-м канале (в градусах)
    float Tmk0;		// Начальная температура МК для коррекции
    float TKUa[6];	//температурные коэффициенты линейной коррекции
    // по напряжениям и токам
    float TKUb[6];	//температурные коэффициенты квадратичной коррекции
    float TKPsi_a[3]; 	//температурные коэффициенты линейной коррекции по tg delta
    float TKPsi_b[3]; 	//температурные коэффициенты квадратичной коррекции по tg delta

        //записываются во флеш базовой платы
    float K_freq;		// коррекция частоты
    float Art;		// коэффициент в канале Pt100, ед.АЦП/Ом
    float Brt;		// смещение в канале Pt100, ед.АЦП
    };

    Bac Bac_block, Bac_newblock;

    struct BdaStruct
    {
        float Ueff_ADC[6];
        float Frequency;
        float Pt100;
     };

    BdaStruct Bda_block;

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

    Bd0 Bd_block0;

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
    int GetExternalData(); // ввод данных в зависимости от выбранного режима и номера опыта
    int SetNewTuneCoefs(); // заполнение Bac_newblock, чтобы не было пурги после настройки
    int SaveWorkConfig();
    int LoadWorkConfig();
    QWidget *Bd1W(QWidget *parent);
    void FillBd1(QWidget *parent);
    void RefreshAnalogValues(int bdnum);
    int TuneChannel();

#endif

    float ToFloat(QString text);

    QString ValuesFormat, WidgetFormat;

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
    void GenerateReport();
    int TunePt100Channel();

#endif
    void SetDefCoefs();

};


#endif // TUNEDIALOG85_H
