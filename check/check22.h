#ifndef CHECK22_H
#define CHECK22_H

#include <QWidget>
#include "../config/config22.h"

#define ACHECKNUMPOINTS  64 // количество точек, по которым выдаются значения в блоке Bda
#define A22_BD  1
#define A22_BDA 2

class Check22
{
public:

    struct Bd1
    {
        float Dmed[AIN22_NUMCH]; // тек. значение уср., град. Цельсия
        float Tmed[AIN22_NUMCH]; // тек. значение уср., Ом
    };

    // Блок Bdа – оцифрованные сигналы в масштабах АЦП и частота в Гц
    struct Bda
    {
        quint32 sin[AIN22_NUMCH];
    };

    Bda Bda_block;
    Bd1 Bd_block;

    Check22();
    QWidget *BdaW(QWidget *parent);
    QWidget *Bd1W(QWidget *parent);
    void FillBda(QWidget *parent);
    void FillBd1W(QWidget *parent);
};

#endif // CHECK22_H
