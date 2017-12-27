#ifndef CHECKDIALOGA1_H
#define CHECKDIALOGA1_H

#include "eabstractcheckdialog.h"
#include "checka1.h"
#include "check.h"

class CheckDialogA1 : public EAbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialogA1(int board, QWidget *parent = 0);

signals:

public slots:

private:
    CheckA1 *ChA1;
    Check *Ch;
    const QString ValuesFormat = "QLabel {border: 1px solid red; border-radius: 4px; padding: 1px; color: red; font: bold 10px;}";
    int NVar;

    QWidget *AutoCheckUI(); // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля
    void RefreshAnalogValues(int bdnum); // обновление полей в GUI из полученного соответствующего Bd_block
    void PrepareHeadersForFile(int row); // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
    void PrepareAnalogMeasurements();
    QWidget *CustomTab();
};

#endif // CHECKDIALOGA1_H
