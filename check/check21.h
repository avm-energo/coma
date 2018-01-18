#ifndef CHECK21_H
#define CHECK21_H

#include <QWidget>
#include "../config/config21.h"

#define ACHECKNUMPOINTS  64 // количество точек, по которым выдаются значения в блоке Bda
#define A21_BD  1
#define A21_BDA 2

class Check21
{
public:

    struct Bd1
    {
        float ain[AIN21_NUMCH]; // тек. значение по каналу (уср., инж.)
        float inI[AIN21_NUMCH]; // тек. значение по каналу (уср., в мА)
        float inU[AIN21_NUMCH]; // тек. значение по каналу (уср., в В)
//        float at; // тек. значение температуры кристалла, °С
//        float ainn[AIN21_NUMCH]; // минимальное значение по каналу (инж.)
//        float ainx[AIN21_NUMCH]; // максимальное значение по каналу (инж.)
    };

    // Блок Bdа – оцифрованные сигналы в масштабах АЦП и частота в Гц
    struct Bda
    {
        float sin[AIN21_NUMCH];
    };

    Bda Bda_block;
    Bd1 Bd_block;

    Check21();
    QWidget *BdaW(QWidget *parent);
    QWidget *Bd1W(QWidget *parent);
    void FillBda(QWidget *parent);
    void FillBd1W(QWidget *parent);
};

#endif // CHECK21_H
