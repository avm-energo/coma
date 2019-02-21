#ifndef EABSTRACTTUNEDIALOGA1DN_H
#define EABSTRACTTUNEDIALOGA1DN_H

#define GOST1983ROWCOUNT    6 // 80, 100, 120 %
#define GOST23625ROWCOUNT   10 // 20, 50, 80, 100, 120 %
#define GOST1983COLCOUNT    10 // K, S, 3x(dU, dP), sU, sP
#define GOST23625COLCOUNT   18 // K, S, 5x(dU, dP), dd, dD, 2x(sU, sP)

#define TUNEA1LEVELS    9
#define TUNEVARIANTSNUM 3

#include "eabstracttunedialog.h"
#include "../gen/report.h"
#include "../config/configa1.h"
#include "../check/checka1.h"

class EAbstractTuneDialogA1DN : public EAbstractTuneDialog
{
    Q_OBJECT

public:

    enum PovTypes
    {
        GOST_NONE, // не задано
        GOST_23625, // по 5 точкам туда-сюда
        GOST_1983 // по 3 точкам только туда
    };

    enum DNTypes
    {
        DNT_OWN, // свой ДН
        DNT_FOREIGN
    };

    enum GetAndAverageTypes
    {
        GAAT_BDA_OUT,
        GAAT_BDA_IN
    };

    enum UModesBlocks
    {
        BLOCK_ALTERNATIVE = 2,
        BLOCK_DIRECT = 3
    };

    struct ReportHeaderStructure
    {
        QString Organization;   // организация, проводившая проверку
        QString Day;            // день месяца проведения проверки
        QString Month;          // месяц
        QString Yr;             // две последние цифры года (20хх)
        QString DNType;         // тип трансформатора (делителя) напряжения
        QString DNNamePhase;    // обозначение ТН и фаза
        QString DNSerNum;       // серийный номер ДН
        QString DNTol;          // класс точности ДН
        QString DNU1;           // номинальное первичное напряжение ДН
        QString DNU2;           // номинальное вторичное напряжение ДН
        QString DNP;            // номинальная мощность нагрузки ДН
        QString DNF;            // номинальная частота ДН
        QString DNOrganization; // предприятие-изготовитель ДН
        QString DNPlace;        // место установки ДН
        QString DNDevices;      // эталонные средства поверки ДН
        QString Temp;           // температура окружающей среды
        QString Humidity;       // влажность воздуха
        QString Pressure;       // атмосферное давление
        QString Voltage;        // напряжение питания сети
        QString Freq;           // частота питания сети
        QString KNI;            // коэффициент искажения синусоидальности кривой напряжения
        QString OuterInsp;      // вывод проведения внешнего осмотра
        QString WindingsInsp;   // вывод проведения проверки правильности обозначения выводов и групп соединений обмоток
        QString PovDateTime;    // дата и время выполнения поверки
    };

    struct PovDevStruct // данные об установке
    {
        QString DevName; // наименование установки
        QString DevSN; // серийный (заводской) номер
        QString DevPrecision; // точность
    };

    struct Baci2
    {
        float U1kDN[6];     // измеренные при калибровке напряжения на выходе своего ДН для значений 0 и вблизи 12, 30, 48, 60 и 72 В
        float U2kDN[6];     // и соответствующие им значения на выходе эталонного делителя
        float PhyDN[6]; 	// фазовый сдвиг ДН на частоте 50 Гц для значений напряжения U1kDN[6]
        float dU_cor[5];    // относительная ампл. погрешность установки после коррекции, в %
        float dPhy_cor[5];  // абс. фазовая погрешность установки после коррекции, мин
        float ddU_cor[5];	// среднеквадратичное отклонение амплитудной погрешности
        float ddPhy_cor[5]; // среднеквадратичное отклонение фазовой погрешности
        float K_DN;         // номинальный коэффициент деления ДН
    };

    struct Bac2
    {
        Baci2 Bac_block2[TUNEVARIANTSNUM];
        quint32 DNFNum;     // заводской номер делителя
    };

    struct Baci3
    {
        float U1kDN[6];     // измеренные при калибровке напряжения на выходе своего ДН для значений 0 и вблизи 20, 50, 80, 100, 120 В
        float U2kDN[6];     // и соответствующие им значения на выходе эталонного делителя
        float dU_cor[5];    // относительная ампл. погрешность установки после коррекции, в %
        float ddU_cor[5];	// среднеквадратичное отклонение амплитудной погрешности
        float K_DN;         // номинальный коэффициент деления ДН
    };

    struct DdStruct
    {
        float dUrms;
        float Phy;
        float sU;
        float sPhy;
    };

    struct Bac3
    {
        Baci3 Bac_block3[TUNEVARIANTSNUM];
        quint32 DNFNum;
    };

    Bac2 Bac_block2;
    Bac3 Bac_block3;
    ConfigA1 *CA1;
    CheckA1 *ChA1;
    bool Accepted;
    int PovNumPoints;
    int Mode; // 0 - переменный, 1 - постоянный ток
    ReportHeaderStructure ReportHeader;
    PovDevStruct PovDev;
    DdStruct Dd_Block[TUNEA1LEVELS];
    int VoltageType, DNType;
//    int Index;
    float CurrentS; // текущее значение нагрузки
    int PovType; // тип поверяемого оборудования (по какому ГОСТу)

    EAbstractTuneDialogA1DN(QWidget *parent = nullptr);

    void InputTuneParameters(int dntype);
#if PROGSIZE != PROGSIZE_EMUL
    void GetBdAndFillMTT();
    int GetAndAverage(int type, void *out, int index); // type = GAAT_BDA_OUT, GAAT_BDA_IN
    int GetBac();
    int AndClearInitialValues();
#endif
    int ShowVoltageDialog(int percent);
    void FillMedian(int index);

    bool ConditionDataDialog();
    bool DNDialog(PovDevStruct &PovDev);
    void LoadSettings();
    void SaveSettings();
    void FillModelRow(int index);
    void FillHeaders();
    void TemplateCheck();
signals:
    void StartPercents(int Percent);
    void SetPercent(int Percent);

public slots:
    void SetTuneParameters();
    void SetConditionData();
    void FillBac(int bacnum);
    void FillBackBac(int bacnum);
    void SetDefCoefs();
//    void AcceptDNData();
    void SetDNData();
    void TempRandomizeModel();

private slots:
    void SetKDNByTuneVariant(int index);

private:
    void FillBackBdIn();
    void FillBdIn();
    void FillBackBdOut();
    void FillBdOut();
};

#endif // EABSTRACTTUNEDIALOGA1DN_H
