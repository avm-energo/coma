#ifndef CHECKDIALOGA1_H
#define CHECKDIALOGA1_H

#include "abstractcheckdialog.h"
#include "checka1.h"

class CheckDialogA1 : public AbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialogA1(QWidget *parent = 0);

signals:

public slots:

private:
    Check_A1 ChA1;
    const QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: blue; font: bold 10px;}";

    QWidget *AutoCheckUI(); // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля
    void RefreshAnalogValues(int bdnum); // обновление полей в GUI из полученного соответствующего Bd_block
    void PrepareHeadersForFile(int row); // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
};

#endif // CHECKDIALOGA1_H
