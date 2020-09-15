#ifndef CHECKDIALOGKIV_H
#define CHECKDIALOGKIV_H

#include "../iec104/ethernet.h"
#include "../iec104/iec104.h"
#include "../modbus/modbus.h"
#include "check.h"
#include "checkkiv.h"
#include "eabstractcheckdialog.h"

class CheckDialogKIV : public EAbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialogKIV(BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = nullptr);

    Check_KIV *ChKIV;

signals:

public slots:
    void SetWarnAlarmColor(QList<bool> WarnAlarm);
    void SetAlarmColor(QList<bool> Alarm);
    void UpdateFlData(IEC104Thread::FlSignals104 *);
    void UpdateSponData(IEC104Thread::SponSignals *);
    void USBUpdate();

private:
    Check *Ch;
    QTimer *BdTimer;
    int BdNum;
    bool XlsxWriting;
    const QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; "
                                 "color: blue; font: bold 10px;}";
    quint8 stColor[7];

    QWidget *AutoCheckUI();   // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля
    void RefreshAnalogValues(int bdnum);  // обновление полей в GUI из полученного
                                          // соответствующего Bd_block
    void PrepareHeadersForFile(int row);  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл
                                          // xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
    void PrepareAnalogMeasurements();
    QWidget *CustomTab();

private slots:
    void StartBdMeasurements();
    void StopBdMeasurements();
    void UpdateModBusData(QList<ModBus::SignalStruct> Signal);
    //    void ErrorRead();
    void onModbusStateChanged();
};
#endif // CHECKDIALOG84_H
