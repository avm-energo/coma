#ifndef CHECKHARMONICKDV_H
#define CHECKHARMONICKDV_H

#include "../gen/maindef.h"
#include "../iec104/ethernet.h"
#include "../iec104/iec104.h"
#include "../modbus/modbus.h"
#include "check.h"

#include <QWidget>

class CheckHarmonicKDV
{
public:
    CheckHarmonicKDV();

    struct Bd_5_7
    {
        float UI_Harm[3][61];
    };

    Bd_5_7 Bd_block5, Bd_block7;

    void FillBd5(QWidget *parent);
    void FillBd7(QWidget *parent);

    void FillBd(QWidget *parent, QString Name, QString Value);

    QWidget *Bd6W(QWidget *parent);
    QWidget *Bd7W(QWidget *parent);
    QWidget *Bd8W(QWidget *parent);
    QWidget *Bd9W(QWidget *parent);
    QWidget *Bd10W(QWidget *parent);
    QWidget *Bd11W(QWidget *parent);
    QWidget *Bd12W(QWidget *parent);
    QWidget *Bd13W(QWidget *parent);
    QWidget *Bd14W(QWidget *parent);
    QWidget *Bd15W(QWidget *parent);
    QWidget *Bd16W(QWidget *parent);
    QWidget *Bd17W(QWidget *parent);
    QWidget *Bd18W(QWidget *parent);

private:
    QString ValuesFormat, WidgetFormat;
};

#endif // CHECKHARMONICKDV_H
