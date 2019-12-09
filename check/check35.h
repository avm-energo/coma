#ifndef CHECK35_H
#define CHECK35_H

#include <QWidget>
#include <QDialog>
#include "eabstractcheckdialog.h"
#include "../config/config35.h"


class Check35 : public QDialog
{
     Q_OBJECT
public:
     explicit Check35();
     ~Check35();


    QWidget *Bd1W(QWidget *parent);
    //void FillBd2(QWidget *parent);
    //void FillBd3(QWidget *parent);
    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля
    QWidget *CustomTab();

signals:

public slots:

private slots:

};

#endif // CHECK35_H

