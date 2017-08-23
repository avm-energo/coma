#ifndef A1DIALOG_H
#define A1DIALOG_H

#include <QDialog>
#include "limereport/lrreportengine.h"
#include "../config/configa1.h"
#include "../check/checka1.h"

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
        float U2kDN[6];     // и соответствующие им значения на выходе эталонного делителя.
        float PhyDN[6]; 	// фазовый сдвиг ДН на частоте 50 Гц для значений напряжения U1kDN[6]
        float dU_cor[5];    // относительная ампл. погрешность установки после коррекции, в %
        float dPhy_cor[5];  // абс. фазовая погрешность установки после коррекции, срад
    };

    Bac Bac_block;

private:
    CheckA1 *ChA1;
    ConfigA1 *CA1;
    LimeReport::ReportEngine *report;
    QVector<publicclass::DataRec> S2Config;
    QTimer *MeasurementTimer;
    struct ResultsStruct
    {
        float K;    // отношение напряжения к номинальному напряжению ТН
        float dUp;  // относительная погрешность прибора по амплитуде
        float dPp;  // абсолютная погрешность прибора по фазе
        float dUd;  // относительная погрешность ДН по амплитуде
        float dPd;  // абсолютная погрешность ДН по фазе
    };
    ResultsStruct Results[9];   // девять уровней напряжения: 20, 50, 80, 100, 120, 100, 80, 50, 20 %
    int Counter;
    int PovType; // тип поверяемого оборудования (по какому ГОСТу)

    void SetupUI();
    int GetConf();
    void FillBdOut();
    void WriteProtocolToFile();
    void ShowProtocol();
    void SaveProtocolToPDF();

private slots:
    void StartWork();
    void MeasTimerTimeout();
    void Accept();
    void Decline();
};

#endif // A1DIALOG_H
