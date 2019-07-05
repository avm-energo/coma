#ifndef CHECKDIALOG31_H
#define CHECKDIALOG31_H

#include "eabstractcheckdialog.h"
#include "../config/config31.h"
#include "check31.h"
#include "check.h"

class CheckDialog31 : public EAbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialog31(BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = nullptr);

signals:

public slots:

private:
    Check31 *Ch31;
    Check *Ch;
    QTimer *BdaTimer;

    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля
    QWidget *CustomTab();
#if PROGSIZE != PROGSIZE_EMUL
    void RefreshAnalogValues(int bdnum); // обновление полей в GUI из полученного соответствующего Bd_block
    void PrepareHeadersForFile(int row); // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
    void PrepareAnalogMeasurements();

private slots:
    void StartBdaMeasurements();
    void StopBdaMeasurements();
    void BdaTimerTimeout();
#endif
};

#endif // CHECKDIALOG31_H
