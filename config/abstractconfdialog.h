#ifndef ABSTRACTCONFDIALOG_H
#define ABSTRACTCONFDIALOG_H

#include <QDialog>
#include "config/config.h"

class AbstractConfDialog : public QDialog
{
    Q_OBJECT
public:
    AbstractConfDialog(QWidget *parent = 0);

    QVector<publicclass::DataRec> *S2Config;

    QWidget *ConfButtons();
    virtual void Fill() = 0;
    virtual void SetDefConf() = 0;
    virtual bool CheckConf() = 0;
    void PrereadConf();
    int GetChNumFromObjectName(QString ObjectName);

public slots:

private:

    virtual void SetupUI() = 0;

private slots:
    void SaveConfToFile();
    void LoadConfFromFile();
    void ReadConf();
    void WriteConf();

signals:
    void BsiIsNeedToBeAcquiredAndChecked(); // signal to reload start block emitted when new configuration has been sent to module
    void NewConfLoaded(); // signal to load configuration in all appropriate windows (main conf, base conf, mez conf)
    void LoadDefConf(); // signal to load default configuration
};

#endif // ABSTRACTCONFDIALOG_H
