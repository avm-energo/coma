#ifndef CHECKDIALOG85_H
#define CHECKDIALOG85_H

#include "eabstractcheckdialog.h"
#include "check85.h"
#include "check.h"

class CheckDialog85 : public EAbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialog85(BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = nullptr);

signals:

public slots:

private:
    Check_85 *Ch85;
    Check *Ch;
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
    void WriteToFile(int row, int bdnum = 0); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
    void PrepareAnalogMeasurements();
    QWidget *CustomTab();
};

#endif // CHECKDIALOG85_H
