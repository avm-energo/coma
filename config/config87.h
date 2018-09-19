#ifndef CONFIG87_H
#define CONFIG87_H

#define CHR87_DEF_PERCENTLIMIT    0
#define CHR87_DEF_T_DOWN          1
#define CHR87_DEF_T_UP            10
#define CHR87_DEF_F_DOWN          16
#define CHR87_DEF_F_UP            25
#define CHR87_DEF_A_DOWN          4
#define CHR87_DEF_A_UP            20

#define CHR87_NUMCH 8

#define CHR87_MTB_STARTIDX  1201

#include <QVector>
#include "config.h"
#include "../gen/modulebsi.h"


class Config87
{
public:
    explicit Config87(QVector<S2::DataRec> &config);

    struct Bci
    {
        qint8 percentLimit[CHR87_NUMCH];  // Количество точек лежащих выше порога, %
        qint16 T_down[CHR87_NUMCH];        // Нижняя граница длительности сигнала, мкс
        qint16 T_up[CHR87_NUMCH];          // Верхняя граница длительности сигнала, мкс
        float F_down[CHR87_NUMCH];         // Нижняя частотная граница, Гц
        float F_up[CHR87_NUMCH];           // Верхняя частотная граница, Гц
        float A_down[CHR87_NUMCH];         // Уровень низкой степени опасности, мВ
        float A_up[CHR87_NUMCH];           // Уровень выскокой степени опасности, мВ

    };

    Bci Bci_block;

    void SetDefConf();
};

#endif // CONFIG87_H
