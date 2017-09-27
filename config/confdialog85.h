#ifndef CONFDIALOG85_H
#define CONFDIALOG85_H

#include <QDialog>

#include "../publicclass.h"
#include "../config/config85.h"
#include "abstractconfdialog.h"

class ConfDialog85 : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfDialog85(QVector<publicclass::DataRec> &S2Config, QWidget *parent = 0);
    ~ConfDialog85();

private:
    Config85 *C85;

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

#endif // CONFDIALOG85_H
