#ifndef CONFDIALOG_H
#define CONFDIALOG_H

#include "config.h"
#include "abstractconfdialog.h"

class ConfDialog : public AbstractConfDialog
{
    Q_OBJECT
public:
    ConfDialog(QVector<S2::DataRec> &S2Config, quint32 MTypeB, quint32 MTypeM, QWidget *parent = nullptr);

    void SetDefConf();
    void Fill();
    void FillBack();
    void CheckConf(){}

public slots:

private:
    Config *ConfigMain;

    void SetupUI();

private slots:
    void Set104(double);
    void SetCType(int);

signals:
};

#endif // CONFDIALOG_H
