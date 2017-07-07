#ifndef CONFDIALOG31_H
#define CONFDIALOG31_H

#include <QDialog>

#include "abstractconfdialog3x.h"
#include "config31.h"

// Описание блока Bci
#define DBCI_INTYPE     11
#define DBCI_DLY        12
#define DBCI_PAIR       16
#define DCONF_NUM       17

class ConfDialog31 : public AbstractConfDialog3x
{
    Q_OBJECT
public:
    explicit ConfDialog31(QVector<publicclass::DataRec> &S2Config, bool BaseBoard=true, QWidget *parent = 0);

private:
    Config31 *C31;

    void Fill();
    void CheckConf();
    void SetDefConf();
    void SetPair(int firstch, int secondch);

signals:
    void BsiIsNeedToBeAcquiredAndChecked();

public slots:

private slots:
    void SetChTypData(int); // задать тип канала (номер канала - в objectName.aplit(".").at(1)
    void SetDly(double); // задать задержку для контроля дребезга
    void SetPair(int); // задать парность каналов
};

#endif // CONFDIALOG31_H
