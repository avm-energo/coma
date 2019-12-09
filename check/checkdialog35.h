#ifndef CHECKDIALOG35_H
#define CHECKDIALOG35_H

#include "eabstractcheckdialog.h"
#include "../config/config35.h"
#include "check35.h"
#include "check.h"

class CheckDialog35 : public EAbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialog35(BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = nullptr);

signals:

public slots:

private:
    Check35 *Ch35;
    Check *Ch;

    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля

#if PROGSIZE != PROGSIZE_EMUL
    void RefreshAnalogValues(int bdnum); // обновление полей в GUI из полученного соответствующего Bd_block
    void PrepareHeadersForFile(int row); // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
    void PrepareAnalogMeasurements(); // функция подготовки к измерениям (например, запрос постоянных данных)
    QWidget *CustomTab();
#endif

private slots:

};

#endif // CHECKDIALOG35_H

