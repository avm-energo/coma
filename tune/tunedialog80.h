#ifndef TUNEDIALOG80_H
#define TUNEDIALOG80_H

#include <QCloseEvent>
#include <QHBoxLayout>
#include <QByteArray>
#include <QStringList>
#include <QStandardItemModel>
#include <QCloseEvent>
#include "limereport/lrreportengine.h"
#include "../config/config80.h"
#include "../iec104/iec104.h"
#include "eabstracttunedialog.h"
#include <QDialog>
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

class TuneDialog80 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialog80(QVector<S2::DataRec> &S2Config, QWidget *parent = nullptr);
signals:
    //void stopall();
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
    enum PovTypes
    {
        GOST_NONE, // не задано
        GOST_23625, // по 5 точкам туда-сюда
        GOST_1983 // по 3 точкам только туда
    };

    bool Cancelled = false, DefConfig;
    Config80 *C80;
    //QVector<S2::DataRec> *S2Config;
    //QVector<S2::DataRec> *S2ConfigForTune;  // для регулировки
    Config80::Bci Bci_block_work;
    iec104 *mipcanal;
    int TuneControlType;
    int SecondsToEnd15SecondsInterval;
    //QHash <QString, int (TuneDialog80::*)()> pf;
    LimeReport::ReportEngine *report;

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

    struct BdaStruct
    {
         float Frequency;            // 2 частота сигналов, Гц,
         float IUefNat_filt[6];      // 3-8 Истинные действующие значения сигналов в вольтах и амперах
         float IUeff_filtered[6];    // 9-14 Действующие значения сигналов по 1-й гармонике
         float phi_next_f[6];        // 15-20 Углы сдвига между сигналами по 1-й гармонике в 6 каналах в градусах,
         float PNatf[3];             // 21-23 Истинная активная мощность, по фазам
         float SNatf[3];             // 24-26 Кажущаяся полная мощность по эффективным  токам и напряжениям,
         float QNatf[3];             // 27-29 Реактивная мощность по кажущейся полной и истинной активной
         float CosPhiNat[3];         // 30-32 cos phi по истинной активной мощности
         float Pf[3];                // 33-35 Активная мощность по 1-й гармонике
         float Qf[3];                // 36-38 Реактивная мощность по 1-й гармонике
         float Sf[3];                // 39-41 Полная мощность по 1-й гармонике
         float CosPhi[3];            // 42-44 cos phi по 1-й гармонике
     };

    BdaStruct Bda_block;

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

    void SetupUI();
    QHBoxLayout *MipPars(int parnum, const QString &groupname);
    void FillBac();
    void FillBackBac();
    void PrepareConsts();
    void FillNewBac();
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
    int StartCheckAnalogValues(double u, double i, double deg, bool tol); // deg - угол в градусах между токами и напряжениями одной фазы, tol - 0: начальная точность, 1 - повышенная
    int CheckAnalogValues(double u, double i, double p, double q, double s, double phi, double cosphi, double utol, double itol, double pht, double pt, double ct);
    bool SetConfA(int i2nom);
    int GetExternalData(); // ввод данных в зависимости от выбранного режима и номера опыта
    void MsgClear();
    int SetNewTuneCoefs(); // заполнение Bac_newblock, чтобы не было пурги после настройки
    int SaveWorkConfig();
    int LoadWorkConfig();
    QWidget *Bd1W(QWidget *parent);
    void FillBd1(QWidget *parent);
    void RefreshAnalogValues(int bdnum);
    void ShowTable();
    void UpdateItemInModel(int row, int column, QVariant value);
    void CheckData(PovDevStruct &PovDev);
    void GenerateReport();
    void ReportDialog(PovDevStruct &PovDev);
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

#endif
    void SetDefCoefs();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // TuneDialog80_H
