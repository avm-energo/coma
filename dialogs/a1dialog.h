#ifndef A1DIALOG_H
#define A1DIALOG_H

#include <QDialog>
#include "../publicclass.h"

class A1Dialog : public QDialog
{
    Q_OBJECT
public:
    explicit A1Dialog(QWidget *parent = 0);

    struct Bac
    {
        float U1kDN[6];     // измеренные при калибровке напряжения на выходе своего ДН для значений вблизи 12, 30, 48, 60 и 72 В
        float U2kDN[6];     // и соответствующие им значения на выходе эталонного делителя.
        float PhyDN[6]; 	// фазовый сдвиг ДН на частоте 50 Гц для значений напряжения U1kDN[6]
        float dU_cor[5];    // относительная ампл. погрешность установки после коррекции, в %
        float dPhy_cor[5];  // абс. фазовая погрешность установки после коррекции, срад
    };

    Bac Bac_block;

private:
    bool Cancelled;
    QTimer *MeasurementTimer;

    void SetupUI();

private slots:
    void StartWork();
    void MeasTimerTimeout();
    void Accept();
    void Decline();
};

#endif // A1DIALOG_H
