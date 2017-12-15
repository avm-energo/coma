#ifndef CHECK21_H
#define CHECK21_H

#include <QWidget>

#define ACHECKNUMPOINTS  64 // количество точек, по которым выдаются значения в блоке Bda
#define A21_BD  1
#define A21_BDA 2

class Check21
{
public:

    struct Bd1
    {
        float ain[16]; // тек. значение по каналу (уср., инж.)
        float inI[16]; // тек. значение по каналу (уср., в мА)
        float inU[16]; // тек. значение по каналу (уср., в В)
        float at; // тек. значение температуры кристалла, °С
        float ainn[16]; // минимальное значение по каналу (инж.)
        float ainx[16]; // максимальное значение по каналу (инж.)
    };

    // Блок Bdа – оцифрованные сигналы в масштабах АЦП и частота в Гц
    struct Bda
    {
        quint32 sin[16];
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
