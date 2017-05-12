#ifndef E_CONFDIALOG_H
#define E_CONFDIALOG_H

#include <QDialog>

#include "../publicclass.h"
#include "../config/config80.h"
#include "confdialog.h"

class confdialog_80 : public QDialog
{
    Q_OBJECT
public:
    explicit confdialog_80(QWidget *parent = 0);
    ~confdialog_80();

private:

    QByteArray confba;
    config_80 *econf;
    ConfDialog *GeneralConf;

    void FillConfData();
    QWidget *UNom(int numunom); // 1 - первая тройка напряжений, 2 - вторая
    QWidget *INom(int numinom); // 1 - первичный ток первой группы, 2 - вторичный ток первой группы, 3,4 - то же по второй группе
    QWidget *Threshold(QString str, int numthr);
    int GetWNum(const QString &ObjName);
    void SetComboBox(QString name, const QString &text);
    void SetSpinBox(QString name, double value);
    void SetCheckBox(QString name, bool isChecked);

signals:
    void BsiIsNeedToBeAcquiredAndChecked();

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
    void GetBci();
    void WriteConfDataToModule();
    void SetupUI();
    void SetDefConf();
    void SetNewConf();
    void LoadConf();
    void SaveConf();
};

#endif // E_CONFDIALOG_H
