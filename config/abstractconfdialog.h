#ifndef ABSTRACTCONFDIALOG_H
#define ABSTRACTCONFDIALOG_H

#include "config/config.h"

#include <QDialog>

class AbstractConfDialog : public QDialog
{
    Q_OBJECT
public:
/*    struct ModConf
    {
        void *Bci_block;
        void *Bci_defblock;
        int Bci_block_size;
        QVector<publicclass::DataRec> S2Config;
    }; */

    AbstractConfDialog(QWidget *parent = 0);
    virtual ~AbstractConfDialog();

    QVector<publicclass::DataRec> S2Config;

    QWidget *Widget104();
    QWidget *ConfButtons();
    virtual void Fill() = 0;
    virtual void SetDefConf() = 0;

public slots:

private:

    virtual void SetupUI() = 0;
    virtual bool CheckConf() = 0;
    void SaveConfToFile();
    void LoadConfFromFile();
    void ReadConf();
    void WriteConf();

private slots:

signals:
    void BsiIsNeedToBeAcquiredAndChecked(); // signal to reload start block emitted when new configuration has been sent to module
    void NewConfLoaded(); // signal to load configuration in all appropriate windows (main conf, base conf, mez conf)
    void LoadDefConf(); // signal to load default configuration
};

#endif // ABSTRACTCONFDIALOG_H
