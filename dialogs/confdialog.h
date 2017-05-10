#ifndef CONFDIALOG_H
#define CONFDIALOG_H

#include "config/config.h"

#include <QDialog>

class ConfDialog : public QDialog
{
    Q_OBJECT
public:
    ConfDialog(QVector<publicclass::DataRec> &S2Config, QObject *parent = 0);
    virtual ~ConfDialog();

    void SetMainDefConf();
    void Fill();

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
