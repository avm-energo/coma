#ifndef CHECKDIALOG80_H
#define CHECKDIALOG80_H

#include "eabstractcheckdialog.h"
#include "check80.h"
#include "check.h"

class CheckDialog80 : public EAbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialog80(BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = 0);

signals:

public slots:

private:
    Check *Ch;
    Check_80 *Ch80;
    QTimer *timer;
    QXlsx::Document *xlsx;
    bool XlsxWriting;
    int WRow;
    QTime *ElapsedTimeCounter;
    const QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: blue; font: bold 10px;}";

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

#endif // CHECKDIALOG80_H
