#ifndef TUNEKIVDIALOG_H
#define TUNEKIVDIALOG_H

#include "../check/checkkiv.h"
#include "../config/config.h"
#include "../config/configkiv.h"
#include "../gen/report.h"
#include "../generaltunedialog.h"
#include "../widgets/udialog.h"
//#include "../iec104/iec104.h"
#include "tunekiv.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>

//#define TUNEFILELENGTH 256

// // voltages
//#define V60 60.0
//#define V57 57.74

// // frequencies
//#define HZ50 50.0

// // currents
//#define I1 1.0
//#define I5 5.0

//#define C5012 5012
//#define C10024 10024
//#define C15036 15036
//#define C2506 2506
//#define C1253 1253
//#define C626 626

// // parameters for GetExtData
//#define TD_GED_U 0x01 // напряжение
//#define TD_GED_I 0x02 // ток
//#define TD_GED_F 0x04 // частота
//#define TD_GED_D 0x08 // угол нагрузки

//#define TD_TMK 25.0 // degrees
//#define TD_VBAT 3.0 // voltage
//#define TD_FREQ 50  // Hz
//#define CONST2PIF 314.15926

class TuneKIVDialog : public GeneralTuneDialog
{
    Q_OBJECT
public:
    explicit TuneKIVDialog(ConfigKIV *ckiv, TuneKIV *tkiv, QWidget *parent = nullptr);

    // signals:
    //    void Send();

public slots:

private:
    //    AVM_KIV::Bci m_Bci_block_work, m_Bci_block_temporary;
    TuneKIV *TKIV;
    ConfigKIV *CKIV;

    //    QDialog *ask;
    //    QLineEdit *ledit;
    //    QTimer *timer;

    //    enum TuneModes
    //    {
    //        TUNEMIP,
    //        TUNERET,
    //        TUNEMAN
    //    };

    LimeReport::ReportEngine *m_Report;

    //    int m_tuneControlType;
    //    bool m_Cancelled = false, m_defConfig;

    void SetupUI();

    //    int m_filterSteps;

    /*struct ReportHeaderStructure
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
    int RowCount, ColumnCount; // количество рядов и столбцов в модели*/

    //    QHBoxLayout *newTunePBLayout(const QString &pbtext, const std::function<void()> fun);

private slots:

    /*  void ReadN();
        int ReadAnalogMeasurements();
        void SetExtData();
        void CancelExtData();
        void CancelTune();
        int TuneTempCor();
        int CalcTuneCoefs();
        void SaveValuesTemp20();
        void SaveValuesTempMinus20();
        void SaveValuesTemp60();
        void CloseAsk(); */
    void GenerateReport();
};

#endif // TUNEKIVDIALOG_H
