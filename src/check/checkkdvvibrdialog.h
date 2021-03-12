#ifndef CHECKDIALOGVIBRKDV_H
#define CHECKDIALOGVIBRKDV_H

#include "abstractcheckdialog.h"

class CheckKDVVibrDialog : public AbstractCheckDialog
{
    Q_OBJECT
public:
    CheckKDVVibrDialog(QWidget *parent = nullptr);

public slots:

private:
    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл

    UWidget *Bd1W();
    UWidget *Bd2W();
    UWidget *Bd3W();

private slots:
};

#endif // CHECKDIALOGVIBRKDV_H
