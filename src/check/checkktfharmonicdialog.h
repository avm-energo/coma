#ifndef CHECKKTFHARMONICDIALOG_H
#define CHECKKTFHARMONICDIALOG_H

#include "abstractcheckdialog.h"

class CheckKTFHarmonicDialog : public AbstractCheckDialog
{
    Q_OBJECT

public:
    CheckKTFHarmonicDialog(QWidget *parent = nullptr);

private:
    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл

    UWidget *BdUWidget(int value, int min, int max);
    UWidget *BdIWidget(int value, int min, int max);

    //    void ChooseValuesToWrite() override;
    //    void SetDefaultValuesToWrite() override;
    //    void PrepareAnalogMeasurements() override;
};

#endif // CHECKKTFHARMONICDIALOG_H
