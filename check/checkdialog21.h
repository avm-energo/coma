#ifndef CHECKDIALOG21_H
#define CHECKDIALOG21_H

#include "eabstractcheckdialog.h"
#include "../config/config21.h"

#define ACHECKNUMPOINTS  64 // количество точек, по которым выдаются значения в блоке Bda

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

private slots:
    void StartBdaMeasurements();
    void StopBdaMeasurements();
};

#endif // CHECKDIALOG21_H
