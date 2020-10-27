#ifndef CHECKDIALOGHARMONICKDV_H
#define CHECKDIALOGHARMONICKDV_H

//#include "../iec104/iec104.h"
#include "abstractcheckdialog.h"
#include "checkharmonickdv.h"

class CheckKDVHarmonicDialog : public AbstractCheckDialog
{
    Q_OBJECT
public:
    CheckKDVHarmonicDialog(QWidget *parent = nullptr);

public slots:
    void SetWarnColor(int position, bool value) override;
    void SetAlarmColor(int position, bool value) override;
    //    void USBUpdate() override;
    //    void ETHUpdate() override;
    //    void MBSUpdate() override;
    void update() override;

private:
    CheckHarmonicKDV *ChHarmKDV;

    QWidget *AutoCheckUI(); // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum) override; // визуализация наборов текущих данных от модуля

    void PrepareHeadersForFile(int row) override; // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл

    void ChooseValuesToWrite() override;
    void SetDefaultValuesToWrite() override;
    void PrepareAnalogMeasurements() override;
    void updateFloatData();

private slots:
    //    void UpdateModBusData(QList<ModBus::SignalStruct> Signal) override;
};

#endif // CHECKDIALOGHARMONICKDV_H
