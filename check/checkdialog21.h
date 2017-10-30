#ifndef CHECKDIALOG21_H
#define CHECKDIALOG21_H

#include "eabstractcheckdialog.h"
#include "../config/config21.h"
#include "check21.h"

class CheckDialog21 : public EAbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialog21(QWidget *parent = 0);

signals:

public slots:

private:
    bool BdMeasurementsActive, BdaMeasurementsActive, OddTimeout;
    QTimer *timer;
    QTime *ElapsedTimeCounter;

    void SetupUI();
    void RefreshAnalogValues(int bdnum); // обновление полей в GUI из полученного соответствующего Bd_block
    void PrepareHeadersForFile(int row); // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
    void PrepareAnalogMeasurements();

private slots:
    void StartBdaMeasurements();
    void StopBdaMeasurements();
};

#endif // CHECKDIALOG21_H
