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

signals:

public slots:

private slots:
    void SetEqType(int);
    void SetDefConf();
    void ShowAdaptParams(bool isAdaptChecked);
};

#endif // CONFDIALOG85_H
