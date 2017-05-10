#ifndef ABSTRACTCONFDIALOG_H
#define ABSTRACTCONFDIALOG_H

#include "config/config.h"

class QDialog;

class AbstractConfDialog : public QDialog
{
    Q_OBJECT
public:
    AbstractConfDialog(QVector<publicclass::DataRec> &S2Config, QObject *parent = 0);
    virtual ~AbstractConfDialog();

    virtual void Fill() = 0;

public slots:

private:
    struct ModConf
    {
        void *Bci_block;
        void *Bci_defblock;
        int Bci_block_size;
        QPointer<QVector<publicclass::DataRec> > S2Config;
    };

    ModConf ModuleConf;
    virtual void SetupUI() = 0;
    virtual void CheckConf() = 0;
    virtual void SetDefConf() = 0;
    void SaveConfToFile();
    void LoadConfFromFile();
    void ReadConf();
    void WriteConf();
    QWidget *ConfButtons();

private slots:

signals:
    void BsiIsNeedToBeAcquiredAndChecked();
    void NewConfLoaded();
    void LoadDefConf();
};

#endif // ABSTRACTCONFDIALOG_H
