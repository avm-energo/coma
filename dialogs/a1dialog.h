#ifndef A1DIALOG_H
#define A1DIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QCloseEvent>
#include "../tune/eabstracttunedialoga1dn.h"
#include "../gen/report.h"
#include "../gen/maindef.h"
#include "../config/configa1.h"
#include "../check/checka1.h"

#define TUNE_POINTSPER  500 // столько миллисекунд должно усредняться при регулировке
#define TUNEVARIANTSNUM 3

class A1Dialog : public EAbstractTuneDialogA1DN
{
    Q_OBJECT
public:
    explicit A1Dialog(const QString &filename = "", QWidget *parent = nullptr);
    ~A1Dialog();

/*    struct Baci
    {
        float U1kDN[6];     // измеренные при калибровке напряжения на выходе своего ДН для значений вблизи 12, 30, 48, 60 и 72 В
        float U2kDN[6];     // и соответствующие им значения на выходе эталонного делителя
        float PhyDN[6]; 	// фазовый сдвиг ДН на частоте 50 Гц для значений напряжения U1kDN[6]
        float dU_cor[5];    // относительная ампл. погрешность установки после коррекции, в %
        float dPhy_cor[5];  // абс. фазовая погрешность установки после коррекции, мин
        float ddU_cor[5];	// среднеквадратичное отклонение амплитудной погрешности
        float ddPhy_cor[5]; // среднеквадратичное отклонение фазовой погрешности
        float K_DN;         // номинальный коэффициент деления ДН
    };

    struct Bac
    {
        Baci Bac_block[TUNEVARIANTSNUM];
        quint32 DNFNum;     // заводской номер делителя
    };

    Bac Bac_block;

    struct DdStruct
    {
        float dUrms;
        float Phy;
        float sU;
        float sPhy;
    };

    DdStruct Dd_Block;

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

    QStringList TableItem;      // строка таблицы с данными вывода
    QList<QStringList *> MainData; // полные данные таблицы для модели

    ReportHeaderStructure ReportHeader; */

private:
    CheckA1 *ChA1;
    ConfigA1 *CA1;
    QVector<S2::DataRec> S2Config;
//    QTimer *MeasurementTimer;
    struct ResultsStruct
    {
        quint64 Time;
        quint32 GOST;
        quint64 SerNum;
        quint64 DNFNum;
        quint32 VarNum;
        float Temp;
        float Humidity;
        float THD;
        float Frequency;
    };
    struct MainDataStruct
    {
        float S; // Значение мощности нагрузки, установленное на i-м испытании
        float U; // Относительное значение напряжения
        float dUp; // Относительная погрешность измерения по rms
        float ddUp; // Среднеквадратическое отклонение dUp
        float dPp; // Абсолютная погрешность по углу
        float ddPp; // Среднеквадратическое отклонение dPp
        float dUd; // Погрешность поверочной установки по rms
        float dPd; // Погрешность поверочной установки по углу
        float ddUd; // Среднеквадратичное отклонение dUd
        float ddPd; // Среднеквадратичное отклонение dPd
    };

/*    struct PovDevStruct // данные об установке
    {
        QString DevName; // наименование установки
        QString DevSN; // серийный (заводской) номер
        QString DevPrecision; // точность
    };

    PovDevStruct PovDev; */

    int Index, Counter;
    float CurrentS; // текущее значение нагрузки
    int PovType, TempPovType; // тип поверяемого оборудования (по какому ГОСТу)
    ReportModel *RepModel; // модель, в которую заносим данные для отчёта

    bool Autonomous; // =1, если производится формирование протокола из файла, =0 - при работе с прибором
    bool TempFromLE, HumFromLE; // =1, если данные в протокол надо брать из поля ввода, =0 - если из прибора
//    bool Cancelled;

    void SetupUI();
#if PROGSIZE != PROGSIZE_EMUL
    int GetConf();
#endif
    void SetLbls() {}; // заполнить список сообщений
    void SetPf() {}; // заполнить список функций настройки
//    void FillBdOut();
//    void FillMedian();
    void WriteProtocolToFile();
    void ShowProtocol();
    void SaveProtocolToPDF();
    void GenerateReport(); // сгенерировать протокол
//    bool ConditionDataDialog(); // задание условий поверки
//    bool DNDialog(PovDevStruct &PovDev); // задание параметров ДН(ТН)
//    void ShowTable();
//    void FillModelRow(int row);
//    void FillHeaders();

signals:
//    void CloseDialog();
//    void StartPercents(quint32 Percent);
//    void SetPercent(int Percent);

private slots:
#if PROGSIZE != PROGSIZE_EMUL
    void StartWork();
//    void MeasTimerTimeout();
    void Accept();
    void Decline();
    void Proceed();
//    int GetStatistics();
#endif
    void Cancel();
    void ParsePKDNFile(const QString &filename);
//    void SetDNData();
//    void SetConditionData();
    void RBToggled();
//    void SetTuneParameters();
    int ReadAnalogMeasurements();

private:
//    int TuneVariant;
    int PovNumPoints;
//    QString OrganizationString; // наименование организации, работающей с программой

//    void InputTuneParameters(int varnum);
//    void LoadSettings();
//    void SaveSettings();

//    void TemplateCheck();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // A1DIALOG_H
