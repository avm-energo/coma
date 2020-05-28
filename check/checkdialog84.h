#ifndef CHECKDIALOG84_H
#define CHECKDIALOG84_H

#include "eabstractcheckdialog.h"
#include "check84.h"
#include "check.h"
#include "../iec104/iec104.h"
#include "../iec104/ethernet.h"
#include "../modbus/modbus.h"

class CheckDialog84 : public EAbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialog84(BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = nullptr, IEC104* channel = nullptr);

    Check_84 *Ch84;

signals:

public slots:   
    void BdTimerTimeout();


private:
    Check *Ch;
    QTimer *BdTimer;
    int BdNum;
    bool XlsxWriting;
    const QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: blue; font: bold 10px;}";
    IEC104* Ch104;

    QWidget *AutoCheckUI(); // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля
#if PROGSIZE != PROGSIZE_EMUL
    void RefreshAnalogValues(int bdnum); // обновление полей в GUI из полученного соответствующего Bd_block
    void PrepareHeadersForFile(int row); // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
    void PrepareAnalogMeasurements();
#endif
    QWidget *CustomTab();


private slots:
#if PROGSIZE != PROGSIZE_EMUL
    void StartBdMeasurements();
    void StopBdMeasurements();
    void UpdateFlData(Parse104::FlSignals104 *);
    void UpdateSponData(Parse104::SponSignals104 *);
    void UpdateSponDataWithTime(Parse104::SponSignalsWithTime *);
    void UpdateBS104Data(Parse104::BS104Signals *);
    void UpdateModBusData(ModBus::SignalStruct *Signal, int size);
    void ErrorRead();
    void onModbusStateChanged(ConnectionStates state);

#endif
};
#endif // CHECKDIALOG84_H
