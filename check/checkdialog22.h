#ifndef CHECKDIALOG22_H
#define CHECKDIALOG22_H

#include "eabstractcheckdialog.h"
#include "../config/config21.h"
#include "check22.h"
#include "check.h"

class CheckDialog22 : public EAbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialog22(BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = 0);

signals:

public slots:

private:
    Check22 *Ch22;
    Check *Ch;
    QTimer *BdaTimer;

    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля
    void RefreshAnalogValues(int bdnum); // обновление полей в GUI из полученного соответствующего Bd_block
    void PrepareHeadersForFile(int row); // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
    void PrepareAnalogMeasurements();
    QWidget *CustomTab();

private slots:
    void StartBdaMeasurements();
    void StopBdaMeasurements();
    void BdaTimerTimeout();
};

#endif // CHECKDIALOG22_H
