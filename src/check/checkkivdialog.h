#pragma once

#include "abstractcheckdialog.h"

class CheckKIVDialog : public AbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckKIVDialog(QWidget *parent = nullptr);
    ~CheckKIVDialog();

private:
    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл
                                                   // xlsx, bdnum - номер блока данных

    //    void ChooseValuesToWrite() override;
    //    void SetDefaultValuesToWrite() override;
    //    void PrepareAnalogMeasurements() override;
    UWidget *Bd1W();
    UWidget *Bd2W();

private slots:
};
