#ifndef CHECKDIALOGKIV_H
#define CHECKDIALOGKIV_H

#include "../gen/colors.h"
#include "abstractcheckdialog.h"
#include "check.h"
//#include "checkkiv.h"

class CheckKIVDialog : public AbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckKIVDialog(QWidget *parent = nullptr);
    ~CheckKIVDialog();

public slots:
    //    void SetAlarmColor(int position, bool value) override;
    //    void SetWarnColor(int position, bool value) override;
    //    void getFloatData();
    //    void getSPData();
    //    void updateFloatData(DataTypes::FloatStruct &fl);
    //    void updateSPData(DataTypes::SinglePointWithTimeStruct &sp);
    //    void MainTWTabClicked(int tabindex);
    //    void setConnections() override;
    //    void reqUpdate() override;

private:
    //    struct HighlightWarnAlarmStruct
    //    {
    //        quint32 fieldnum;
    //        QString color;
    //    };
    //    QMap<int, QList<UWidget::HighlightWarnAlarmStruct>> m_Bd1HighlightMap()
    //    {
    //        QMap<int, QList<HighlightWarnAlarmStruct>> map;
    //        map[3011] = { { 1000, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3012] = { { 1001, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3013] = { { 1002, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3014] = { { 1100, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3015] = { { 1101, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3016] = { { 1102, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3018] = { { 1000, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3019] = { { 1001, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3020] = { { 1002, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3021] = { { 2429, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3022] = { { 2430, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3023] = { { 2431, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3024] = { { 2429, Colors::ColorsMap()[Colors::Red] } };
    //        map[3025] = { { 2430, Colors::ColorsMap()[Colors::Red] } };
    //        map[3026] = { { 2431, Colors::ColorsMap()[Colors::Red] } };
    //        map[3027] = { { 2426, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3028] = { { 2427, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3029] = { { 2428, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3030] = { { 2426, Colors::ColorsMap()[Colors::Red] } };
    //        map[3031] = { { 2427, Colors::ColorsMap()[Colors::Red] } };
    //        map[3032] = { { 2428, Colors::ColorsMap()[Colors::Red] } };
    //        map[3034] = { { 2432, Colors::ColorsMap()[Colors::AConfO] } };
    //        map[3035] = { { 2432, Colors::ColorsMap()[Colors::Red] } };
    //        return map;
    //    };

    //    struct BdQuery
    //    {
    //        quint32 sigAdr;
    //        quint32 sigQuantity;
    //    };
    //    const QList<BdQuery> FloatBdQueryList { { 101, 2 }, { 1000, 16 }, { 1100, 16 }, { 2400, 7 }, { 2420, 14 },
    //        { 4501, 2 } };
    //    const QList<BdQuery> SpBdQueryList { { 3011, 25 } };

    //    CheckKIV *ChKIV;
    //    QBitArray m_stColor;

    //    QWidget *AutoCheckUI();            // UI для автоматической проверки модуля
    //    QWidget *BdUI(int bdnum) override; // визуализация наборов текущих данных от модуля
    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл
                                                   // xlsx, bdnum - номер блока данных
                                                   //    void ChooseValuesToWrite() override;
                                                   //    void SetDefaultValuesToWrite() override;
                                                   //    void PrepareAnalogMeasurements() override;
    UWidget *Bd1W();
    UWidget *Bd2W();

    //    QWidget *CustomTab() override;

private slots:
    //    void UpdateModBusData(QList<ModBus::SignalStruct> Signal) override;
};
#endif // CHECKDIALOG84_H
