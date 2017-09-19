#ifndef CONFDIALOG35_H
#define CONFDIALOG35_H

#include <QDialog>

#include "abstractconfdialog3x.h"
#include "config31.h"

// Описание блока Bci
#define DBCI_INTYPE     11
#define DBCI_DLY        12
#define DBCI_PAIR       16
#define DCONF_NUM       17

class ConfDialog35 : public AbstractConfDialog3x
{
    Q_OBJECT
public:
    explicit ConfDialog35(QVector<publicclass::DataRec> &S2Config, bool BaseBoard=true, QWidget *parent = 0);

private:
    publicclass::DataRec Config[DCONF_NUM];
    Config31 *C31;

    void Fill();
    void CheckConf();
    void SetPair(int firstch, int secondch);

signals:
    void BsiIsNeedToBeAcquiredAndChecked();

public slots:

private slots:
    void SetChTypData(int); // задать тип канала (номер канала - в objectName.aplit(".").at(1)
    void SetDly(double); // задать задержку для контроля дребезга
    void SetPair(int); // задать парность каналов
};

#endif // CONFDIALOG35_H
