#ifndef CHEKDIALOGKTF_H
#define CHEKDIALOGKTF_H

#include "abstractcheckdialog.h"
#include "checkktf.h"

class CheckKTFDialog : public AbstractCheckDialog
{
    Q_OBJECT

public:
    CheckKTFDialog(QWidget *parent = nullptr);
    ~CheckKTFDialog();

public slots:
    void SetAlarmColor(int position, bool value) override;
    void SetWarnColor(int position, bool value) override;
    void USBUpdate() override;
    void ETHUpdate() override;
    void MBSUpdate() override;

private:
    CheckKTF *ChKTF;

    QWidget *AutoCheckUI();            // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum) override; // визуализация наборов текущих данных от модуля
    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл
                                                   // xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite() override;
    void SetDefaultValuesToWrite() override;
    void PrepareAnalogMeasurements() override;
    void updateFloatData();

private slots:
    void UpdateModBusData(QList<ModBus::SignalStruct> Signal) override;
};

#endif // CHEKDIALOGKTF_H
