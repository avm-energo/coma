#ifndef TUNEKIVDIALOG_H
#define TUNEKIVDIALOG_H

#include "../../models/report.h"
#include "../../widgets/udialog.h"
#include "../generaltunedialog.h"
#include "tunekivadc.h"
#include "tunekivcheck.h"
#include "tunekivr.h"
#include "tunekivtemp60.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>

class TuneKIVDialog : public GeneralTuneDialog
{
    Q_OBJECT
public:
    explicit TuneKIVDialog(ConfigV *config, QWidget *parent = nullptr);

public slots:

private:
    TuneKIVCheck *TKIVCheckDialog;
    TuneKIVADC *TKIVADCUDialog, *TKIVADCIDialog;
    TuneKIVR *TKIVRDialog;
    void prepareReport() override;

    //    LimeReport::ReportEngine *m_Report;

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
    int RowCount, ColumnCount; // количество рядов и столбцов в модели
    */

private slots:

    //    void GenerateReport();
};

#endif // TUNEKIVDIALOG_H
