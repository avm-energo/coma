#ifndef CONFDIALOG35_H
#define CONFDIALOG35_H

#define D35_NUMCH   4

#include <QDialog>

#include "abstractconfdialog3x.h"
#include "config35.h"

class ConfDialog35 : public AbstractConfDialog3x
{
    Q_OBJECT
public:
    explicit ConfDialog35(QVector<publicclass::DataRec> &S2Config, bool BaseBoard=true, QWidget *parent = 0);

private:
    Config35 *C35;

    void Fill();
    void FillBack();
    void CheckConf();
    void SetDefConf();

signals:
    void BsiIsNeedToBeAcquiredAndChecked();

public slots:

private slots:
    void SetChTypData(int); // задать тип канала (номер канала - в objectName.aplit(".").at(1)
    void SetDly(double); // задать задержку для контроля дребезга
    void SetPair(int); // задать парность каналов
};

#endif // CONFDIALOG35_H
