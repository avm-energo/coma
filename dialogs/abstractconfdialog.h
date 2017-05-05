#ifndef ABSTRACTCONFDIALOG_H
#define ABSTRACTCONFDIALOG_H

#include "config/config.h"

class QDialog;

class AbstractConfDialog : public QDialog
{
    Q_OBJECT
public:
    AbstractConfDialog(QObject *parent = 0);
    virtual ~AbstractConfDialog();

    QWidget *Widget104();

public slots:

private:
    Config *ConfigMain;

    virtual void SetupUI() = 0;
    virtual void Fill() = 0;
    virtual void ReadConf() = 0;
    virtual void CheckConf() = 0;
    virtual void WriteConfDataToModule() = 0;
    virtual void SetDefConf() = 0;
    virtual void SaveConfToFile() = 0;
    virtual void LoadConfFromFile() = 0;

private slots:
    void Set104(double);
    void SetCType(int);
    void Fill104();

signals:
    void BsiIsNeedToBeAcquiredAndChecked();
};

#endif // ABSTRACTCONFDIALOG_H
