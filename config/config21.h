#ifndef CONFIG21_H
#define CONFIG21_H

#define AIN21_NUMCH 8

#define AIN21_DEF_OSCDLY    0
#define AIN21_DEF_OSCTHR    0.05f
#define AIN21_DEF_FILTERN   10
#define AIN21_DEF_FILTERTHR 16
#define AIN21_DEF_HYSTERS   0.02f
#define AIN21_DEF_INMIN     4
#define AIN21_DEF_INMAX     20
#define AIN21_DEF_INVMIN    0
#define AIN21_DEF_INVMAX    1000
#define AIN21_DEF_SETMINMIN 10
#define AIN21_DEF_SETMIN    50
#define AIN21_DEF_SETMAX    950
#define AIN21_DEF_SETMAXMAX 990
#define AIN21_DEF_OSC       0

#define AIN21_MTB_STARTIDX  101
#define AIN21_MTM_STARTIDX  121

#define AIN21_AIT_NONE      0
#define AIN21_AIT_MA        1
#define AIN21_AIT_V         2

#include "config.h"
#include <QVector>

class Config21
{
public:
    Config21(QVector<S2::DataRec> &config, bool BaseBoard=true);

    struct Bci
    {
        qint8 in_type[AIN21_NUMCH];     // Тип входа (0 - не исп., 1 - мА, 2 - В, 3 - резерв)
        float in_min[AIN21_NUMCH];      // Минимальное значение диапазона изменения сигнала
        float in_max[AIN21_NUMCH];      // Максимальное значение диапазона
        float in_vmin[AIN21_NUMCH];     // Минимальное значение сигнала в инженерных единицах
        float in_vmax[AIN21_NUMCH];     // Максимальное значение сигнала в инженерных единицах
        float setminmin[AIN21_NUMCH];   // Значение аварийной уставки по минимальному значению
        float setmin[AIN21_NUMCH];      // Значение предупредительной уставки по минимальному значению
        float setmax[AIN21_NUMCH];      // Значение предупредительной уставки по максимальному значению
        float setmaxmax[AIN21_NUMCH];	// Значение аварийной уставки по максимальному значению
        quint8 osc[AIN21_NUMCH];        // Признаки источников запуска осциллограмм
        quint16 oscdly;                 // Задержка в мс начала фиксации максимумов
        float oscthr;                   // Доля от диапазона, при превышении которой начинается запись осциллограмм
        float filterthr;                // Доля от диапазона, при превышении которой между замерами фильтр не работает
        float filtern;                  // Степень фильтрации (0 - отсутствует)
        float hysteresis;               // Значение гистерезиса на сравнение с уставками (доля от диапазона)
    };

    Bci Bci_block;

    void SetDefConf();
};

#endif // CONFIG21_H
