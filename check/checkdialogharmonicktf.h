#ifndef CHECKDIALOGHARMONICKTF_H
#define CHECKDIALOGHARMONICKTF_H

#include "../iec104/iec104.h"
#include "checkharmonicktf.h"
#include "eabstractcheckdialog.h"

class CheckDialogHarmonicKTF : public EAbstractCheckDialog
{
    Q_OBJECT
public:
    CheckDialogHarmonicKTF(BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = nullptr);

public slots:
    void SetPredAlarmColor(quint8 *);
    void SetAlarmColor(quint8 *Alarm);
    void UpdateFlData(IEC104Thread::FlSignals104 *);
    void UpdateSponData(IEC104Thread::SponSignals *);
    void USBUpdate() override;

private:
    CheckHarmonicKTF *ChHarmKTF;
    QWidget *AutoCheckUI();            // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum) override; // визуализация наборов текущих данных от модуля
    void RefreshAnalogValues(int bdnum) override; // обновление полей в GUI из полученного

    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл

    void ChooseValuesToWrite() override;
    void SetDefaultValuesToWrite() override;
    void PrepareAnalogMeasurements() override;
    // QWidget *CustomTab() override;

private slots:
    void UpdateModBusData(QList<ModBus::SignalStruct> Signal) override;
};

#endif // CHECKDIALOGHARMONICKTF_H
