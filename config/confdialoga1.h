#ifndef CONFDIALOGA1_H
#define CONFDIALOGA1_H

#include <QDialog>

#include "../publicclass.h"
#include "../config/configa1.h"
#include "abstractconfdialog.h"

class ConfDialogA1 : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfDialogA1(QVector<publicclass::DataRec> &S2Config, QWidget *parent = 0);
    ~ConfDialogA1();

private:
    ConfigA1 *CA1;

    void Fill();
    void FillBack();
    void SetupUI();
    void CheckConf();

    QWidget *UNom(int numunom); // 1 - первая тройка напряжений, 2 - вторая
    QWidget *INom(int numinom); // 1 - первичный ток первой группы, 2 - вторичный ток первой группы, 3,4 - то же по второй группе
    QWidget *Threshold(QString str, int numthr);

signals:

public slots:

private slots:
    void TokrChoosed(int);
    void HumidityChoosed(int);
    void SetThreshold(double);
    void SetVoltageClass(int);

    void SetCurrent(double);
    void SetEqType(int);
    void SetNPoints(QString);
    void SetNFiltr(int);
    void SetNHFiltr(int);
    void SetDefConf();
};

#endif // CONFDIALOGA1_H
