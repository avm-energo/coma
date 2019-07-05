#ifndef CHECK31_H
#define CHECK31_H

#include <QWidget>
#include "../config/config31.h"

#define A31_NUMCH   11
#define A31_BD  1

class Check31
{
public:

    // Блок Bdа – состояние входных сигналов
    struct Bda
    {
        quint32 chdata;
    };

    Bda Bda_block;

    Check31();
    QWidget *BdaW(QWidget *parent);
    void FillBda(QWidget *parent);
};

#endif // CHECK31_H
