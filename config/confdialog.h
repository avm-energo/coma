#ifndef CONFDIALOG_H
#define CONFDIALOG_H

#include "config/config.h"
#include "abstractconfdialog.h"

class ConfDialog : public AbstractConfDialog
{
    Q_OBJECT
public:
    ConfDialog(QVector<publicclass::DataRec> &S2Config, QWidget *parent = 0);

    void SetDefConf();
    void Fill();
    bool CheckConf()
    {
        return true;
    }

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
