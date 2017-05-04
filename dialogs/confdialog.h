#ifndef CONFDIALOG_H
#define CONFDIALOG_H

#include <QObject>

#include "../publicclass.h"
#include "config/config.h"

class confdialog : public QObject
{
    Q_OBJECT
public:
    confdialog(Bci_Main *bci, QObject *parent = 0);
    ~confdialog();

    QWidget *Widget104();

private:

    Bci_Main *BciMain;
    QWidget *MainWin;

signals:

public slots:
    void Fill();
//    void Fill104(Bci_Main &bcim);

private slots:
    void Set104(double);
    void SetCType(int);
};

#endif // CONFDIALOG_H
