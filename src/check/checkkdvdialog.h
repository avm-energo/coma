#ifndef CHECKDIALOGKDV_H
#define CHECKDIALOGKDV_H

#include "abstractcheckdialog.h"

class CheckKDVDialog : public AbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckKDVDialog(QWidget *parent = nullptr);

private:
    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл
                                                   // xlsx, bdnum - номер блока данных

    //    void ChooseValuesToWrite() override;
    //    void SetDefaultValuesToWrite() override;
    //    void PrepareAnalogMeasurements() override;

    UWidget *Bd1W();
    UWidget *Bd2W();
    UWidget *Bd3W();
    UWidget *Bd4W();
    UWidget *Bd5W();
    //    UWidget *Bd6W();
    //    UWidget *Bd7W();
    //    UWidget *Bd8W();
};

#endif // CHECKDIALOGKDV_H
