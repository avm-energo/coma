#ifndef CHECKDIALOGHARMONICKTF_H
#define CHECKDIALOGHARMONICKTF_H

#include "../iec104/iec104.h"
#include "abstractcheckdialog.h"
#include "checkharmonicktf.h"

class CheckKTFHarmonicDialog : public AbstractCheckDialog
{
    Q_OBJECT
public:
    CheckKTFHarmonicDialog(QWidget *parent = nullptr);

public slots:
    void SetWarnColor(int position, bool value) override;
    void SetAlarmColor(int position, bool value) override;
    void UpdateFlData(IEC104Thread::FlSignals104 *);
    void UpdateSponData(IEC104Thread::SponSignals *);
    void USBUpdate() override;

private:
    CheckHarmonicKTF *ChHarmKTF;
    QWidget *AutoCheckUI(); // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum) override; // визуализация наборов текущих данных от модуля
    void RefreshAnalogValues(int bdnum) override; // обновление полей в GUI из полученного

    void PrepareHeadersForFile(int row) override; // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл

    void ChooseValuesToWrite() override;
    void SetDefaultValuesToWrite() override;
    void PrepareAnalogMeasurements() override;
    // QWidget *CustomTab() override;

private slots:
    void UpdateModBusData(QList<ModBus::SignalStruct> Signal) override;
};

#endif // CHECKDIALOGHARMONICKTF_H
