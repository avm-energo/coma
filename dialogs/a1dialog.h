#ifndef A1DIALOG_H
#define A1DIALOG_H

#include <QDialog>
#include "../publicclass.h"
#include "../config/configa1.h"

class A1Dialog : public QDialog
{
    Q_OBJECT
public:
    explicit A1Dialog(QWidget *parent = 0);

private:
    ConfigA1 *CA1;
    QVector<publicclass::DataRec> S2Config;
    bool Cancelled;
    QTimer *MeasurementTimer;

    void SetupUI();

private slots:
    void StartWork();
    void MeasTimerTimeout();
};

#endif // A1DIALOG_H
