#ifndef A1DIALOG_H
#define A1DIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "limereport/lrreportengine.h"
#include "../config/configa1.h"
#include "../check/checka1.h"

#define GOST1983ROWCOUNT    6 // 80, 100, 120 %
#define GOST23625ROWCOUNT   10 // 20, 50, 80, 100, 120 %
#define GOST1983COLCOUNT    12 // K, S, 3x(dU, dP), ddU, ddP, sU, sP
#define GOST23625COLCOUNT   22 // K, S, 5x(dU, dP), dd, dD, 2x(ddU, ddP, sU, sP)

#define TUNE_COUNTEND   120 // столько точек по
#define TUNE_POINTSPER  500 // столько миллисекунд должно усредняться при регулировке

class A1Dialog : public QDialog
{
    Q_OBJECT
public:
    explicit A1Dialog(QWidget *parent = 0);

    enum PovTypes
    {
        GOST_NONE, // не задано
        GOST_23625, // по 5 точкам туда-сюда
        GOST_1983 // по 3 точкам только туда
    };

    struct Bac
    {
        float U1kDN[6];     // измеренные при калибровке напряжения на выходе своего ДН для значений вблизи 12, 30, 48, 60 и 72 В
        float U2kDN[6];     // и соответствующие им значения на выходе эталонного делителя
        float PhyDN[6]; 	// фазовый сдвиг ДН на частоте 50 Гц для значений напряжения U1kDN[6]
        float dU_cor[5];    // относительная ампл. погрешность установки после коррекции, в %
        float dPhy_cor[5];  // абс. фазовая погрешность установки после коррекции, срад
        float ddU_cor[5];	// среднеквадратичное отклонение амплитудной погрешности
        float ddPhy_cor[5]; // среднеквадратичное отклонение фазовой погрешности
        float K_DN;         // номинальный коэффициент деления ДН
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
    };

    QStringList TableItem;      // строка таблицы с данными вывода
    QList<QStringList *> MainData; // полные данные таблицы для модели

    ReportHeaderStructure ReportHeader;

private:
    CheckA1 *ChA1;
    ConfigA1 *CA1;
    LimeReport::ReportEngine *report;
    QVector<publicclass::DataRec> S2Config;
    QTimer *MeasurementTimer;
/*    struct ResultsStruct
    {
        float K;    // отношение напряжения к номинальному напряжению ТН
        float dUp;  // относительная погрешность прибора по амплитуде
        float dPp;  // абсолютная погрешность прибора по фазе
        float dUd;  // относительная погрешность ДН по амплитуде
        float dPd;  // абсолютная погрешность ДН по фазе
    }; */
//    ResultsStruct Results[9];   // девять уровней напряжения: 20, 50, 80, 100, 120, 100, 80, 50, 20 % или три уровня: 80, 100, 120 % в зависимости от ГОСТа
    int Index, Counter;
    float CurrentS; // текущее значение нагрузки
    int PovType, TempPovType; // тип поверяемого оборудования (по какому ГОСТу)
    QStandardItemModel *ReportModel; // модель, в которую заносим данные для отчёта
    int RowCount, ColumnCount; // количество рядов и столбцов в модели

    void SetupUI();
    int GetConf();
    void FillBdOut();
    void FillMedian();
    void WriteProtocolToFile();
    void ShowProtocol();
    void SaveProtocolToPDF();
    void GenerateReport(); // сгенерировать протокол
    void ConditionDataDialog(); // задание условий поверки
    void DNDialog(); // задание параметров ДН(ТН)
    void UpdateItemInModel(int row, int column, QVariant value);
    void ShowTable();

signals:
    void CloseDialog();
    void StartPercents(quint32 Percent);
    void SetPercent(quint32 Percent);

private slots:
    void StartWork();
    void ParsePKDNFile();
    void MeasTimerTimeout();
    void Accept();
    void Decline();
    void Proceed();
    void Cancel();
    void SetDNData();
    void SetConditionData();
    void RBToggled();
    int GetStatistics();
};

#endif // A1DIALOG_H
