#ifndef CHECKHARMONICKTF_H
#define CHECKHARMONICKTF_H

#include "../gen/board.h"
#include "../iec104/ethernet.h"
#include "../iec104/iec104.h"
#include "../modbus/modbus.h"
#include "check.h"

#include <QWidget>

class CheckHarmonicKTF
{
public:
    CheckHarmonicKTF();

    struct Bd5
    {
        float UI_Harm[3][61];
    };

    struct Bd7
    {
        float UI_Harm[3][61];
    };

    Bd7 Bd_block7;
    Bd5 Bd_block5;

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

#endif // CHECKHARMONICKTF_H
