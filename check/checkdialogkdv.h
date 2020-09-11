#ifndef CHECKDIALOGKDV_H
#define CHECKDIALOGKDV_H

#include "../iec104/ethernet.h"
#include "../iec104/iec104.h"
#include "../modbus/modbus.h"
#include "check.h"
#include "checkkdv.h"
#include "eabstractcheckdialog.h"

class CheckDialogKDV : public EAbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialogKDV(BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = nullptr);

    CheckKDV *ChKDV;

    void USBUpdate();

public slots:
    void SetPredAlarmColor(quint8 *);
    void SetAlarmColor(quint8 *Alarm);
    void UpdateFlData(IEC104Thread::FlSignals104 *);
    void UpdateSponData(IEC104Thread::SponSignals *);

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
    void onModbusStateChanged();
};

#endif // CHECKDIALOGKDV_H
