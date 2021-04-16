#ifndef CHECKDIALOGHARMONICKDV_H
#define CHECKDIALOGHARMONICKDV_H

#include "abstractcheckdialog.h"

class CheckKDVHarmonicDialog : public AbstractCheckDialog
{
    Q_OBJECT
public:
    CheckKDVHarmonicDialog(QWidget *parent = nullptr);

public slots:

private:
    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл

    UWidget *BdUWidget(unsigned int value, int min, int max);
    UWidget *BdIWidget(unsigned int value, int min, int max);

    //    void ChooseValuesToWrite() override;
    //    void SetDefaultValuesToWrite() override;
    //    void PrepareAnalogMeasurements() override;
};

#endif // CHECKDIALOGHARMONICKDV_H
