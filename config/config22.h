#ifndef CONFIG22_H
#define CONFIG22_H

#define AIN22_NUMCH 8

#define AIN22_DEF_KFFILTRT  16
#define AIN22_DEF_CNTFILTRT 10
#define AIN22_DEF_HYSTERS   0
#define AIN22_DEF_RZEROT    100
#define AIN22_DEF_INMIN     -50
#define AIN22_DEF_INMAX     150
#define AIN22_DEF_INVMIN    0
#define AIN22_DEF_INVMAX    1000
#define AIN22_DEF_SETMINMIN -40
#define AIN22_DEF_SETMIN    -25
#define AIN22_DEF_SETMAX    90
#define AIN22_DEF_SETMAXMAX 100
#define AIN22_DEF_W100      1.385f

#define AIN22_MTB_STARTIDX  301
#define AIN22_MTM_STARTIDX  321

#include "config.h"
#include "../gen/modulebsi.h"

class Config22
{
public:
    Config22(QVector<S2::DataRec> &config, int BaseBoard=BoardTypes::BT_BASE);

    struct Bci
    {
        float W100[AIN22_NUMCH];        // Температурный коэффициент
        float in_minT[AIN22_NUMCH];     // Минимальное значение сигнала в градусах
        float in_maxT[AIN22_NUMCH];     // Максимальное значение сигнала в градусах
        float setminminT[AIN22_NUMCH];  // Значение аварийной уставки по минимальному значению
        float setminT[AIN22_NUMCH];     // Значение предупредительной уставки по минимальному значению
        float setmaxT[AIN22_NUMCH];     // Значение предупредительной уставки по максимальному значению
        float setmaxmaxT[AIN22_NUMCH];  // Значение аварийной уставки по максимальному значению
        float kf_filtrT;                // Пороговое значение, при превышении которого между замерами фильтр не работает
        float cnt_filtrT;              // Степень фильтрации (0 - отсутствует)
        float hysteresisT;              // Значение гистерезиса на сравнение с уставками (доля от диапазона)
        quint32 RzeroT[AIN22_NUMCH];    // Сопротивление при 0 градусов
    };

    Bci Bci_block;

    void SetDefConf();
};

#endif // CONFIG22_H
