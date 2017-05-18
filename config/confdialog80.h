#ifndef CONFDIALOG80_H
#define CONFDIALOG80_H

#include <QDialog>

#include "../publicclass.h"
#include "../config/config80.h"
#include "abstractconfdialog.h"

class ConfDialog80 : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfDialog80(QWidget *parent = 0);
    ~ConfDialog80();

private:
    Config80 *C80;

    void Fill();
    void SetupUI();
    bool CheckConf() = 0;

    QWidget *UNom(int numunom); // 1 - первая тройка напряжений, 2 - вторая
    QWidget *INom(int numinom); // 1 - первичный ток первой группы, 2 - вторичный ток первой группы, 3,4 - то же по второй группе
    QWidget *Threshold(QString str, int numthr);

signals:

public slots:

private slots:
    void SetOsc(int);
    void SetThreshold(double);
    void SetVoltageClass(int);
    void SetCurrent(double);
    void SetEqType(int);
    void SetNPoints(QString);
    void SetNFiltr(int);
    void SetNHFiltr(int);
    void SetDefConf();
};

#endif // CONFDIALOG80_H
