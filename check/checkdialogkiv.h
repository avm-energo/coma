#ifndef CHECKDIALOGKIV_H
#define CHECKDIALOGKIV_H

#include "../iec104/iec104.h"
#include "abstractcheckdialog.h"
#include "check.h"
#include "checkkiv.h"

class CheckDialogKIV : public AbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialogKIV(BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = nullptr);
    ~CheckDialogKIV();
    QWidget *EParent;

public slots:
    void SetWarnColor(QList<bool> WarnAlarm);
    void SetAlarmColor(QList<bool> Alarm);
    void UpdateFlData(IEC104Thread::FlSignals104 *);
    void UpdateSponData(IEC104Thread::SponSignals *);
    void USBUpdate() override;
    //    void MainTWTabClicked(int tabindex);

private:
    Check_KIV *ChKIV;
    QWidget *AutoCheckUI();            // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum) override; // визуализация наборов текущих данных от модуля
    void RefreshAnalogValues(int bdnum) override;  // обновление полей в GUI из полученного
                                                   // соответствующего Bd_block
    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл
                                                   // xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite() override;
    void SetDefaultValuesToWrite() override;
    void PrepareAnalogMeasurements() override;
    QWidget *CustomTab() override;

private slots:
    void UpdateModBusData(QList<ModBus::SignalStruct> Signal) override;
};
#endif // CHECKDIALOG84_H
