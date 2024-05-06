#pragma once

#include <tune/generaltunedialog.h>

class TuneKIVDialog final : public GeneralTuneDialog
{
    Q_OBJECT
public:
    explicit TuneKIVDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    void prepareReport();

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
