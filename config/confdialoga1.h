#ifndef CONFDIALOGA1_H
#define CONFDIALOGA1_H

#include <QDialog>

#include "../config/configa1.h"
#include "abstractconfdialog.h"

class ConfDialogA1 : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfDialogA1(QVector<S2::DataRec> &S2Config, QWidget *parent = 0);
    ~ConfDialogA1();

private:
    ConfigA1 *CA1;

    void Fill();
    void FillBack();
    void SetupUI();
    void CheckConf();

signals:

public slots:

private slots:
    void SetDefConf();
};

#endif // CONFDIALOGA1_H
