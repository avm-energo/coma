#ifndef CHECKREMPMODBUSDIALOG_H
#define CHECKREMPMODBUSDIALOG_H

#include "eabstractcheckdialog.h"
#include "../iec104/iec104.h"
#include "../iec104/ethernet.h"
#include "../modbus/modbus.h"

class checktempmodbusdialog: public EAbstractCheckDialog
{
    Q_OBJECT
public:
    explicit checktempmodbusdialog(BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = nullptr);

    checktempmodbusdialog *Ch84;
    bool connectionStateRTU;

    QWidget *Bd1W(QWidget *parent);
    void FillBd(QWidget *parent, QString Name, QString Value);
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

signals:

public slots:


private:
    QString ValuesFormat, WidgetFormat;

private slots:
    void onModbusStateChanged(QModbusDevice::State *state);

};
#endif
