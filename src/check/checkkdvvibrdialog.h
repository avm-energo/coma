#ifndef CHECKDIALOGVIBRKDV_H
#define CHECKDIALOGVIBRKDV_H

//#include "../iec104/iec104.h"
#include "abstractcheckdialog.h"
#include "checkvibrkdv.h"

class CheckKDVVibrDialog : public AbstractCheckDialog
{
    Q_OBJECT
public:
    CheckKDVVibrDialog(QWidget *parent = nullptr);

public slots:
    //    void SetWarnColor(int position, bool value) override;
    //    void SetAlarmColor(int position, bool value) override;
    //    void updateFloatData();
    //    void USBUpdate() override;
    //    void ETHUpdate() override;
    //    void MBSUpdate() override;
    //    void reqUpdate() override;

private:
    CheckVibrKDV *ChVibrKDV;
    //    QWidget *AutoCheckUI();            // UI для автоматической проверки модуля
    //    QWidget *BdUI(int bdnum) override; // визуализация наборов текущих данных от модуля

    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл

    UWidget *Bd1W();
    UWidget *Bd2W();
    UWidget *Bd3W();

    //    void ChooseValuesToWrite() override;
    //    void SetDefaultValuesToWrite() override;
    //    void PrepareAnalogMeasurements() override;

private slots:
    //    void UpdateModBusData(QList<ModBus::SignalStruct> Signal) override;
};

#endif // CHECKDIALOGVIBRKDV_H