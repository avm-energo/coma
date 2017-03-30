#ifndef CONFIG21_H
#define CONFIG21_H

#define ABCI_INTYPE8    101
#define ABCI_INMIN8     102
#define ABCI_INMAX8     103
#define ABCI_INVMIN8    104
#define ABCI_INVMAX8    105
#define ABCI_SETMINMIN8 106
#define ABCI_SETMIN8    107
#define ABCI_SETMAX8    108
#define ABCI_SETMAXMAX8 109
#define ABCI_OSC8       110
#define ABCI_OSCDLY     111
#define ABCI_OSCTHR     112
#define ABCI_FILTERTHR  113
#define ABCI_FILTERN    114
#define ABCI_HYSTERESIS 115

#define CONF_NUM21          16
#define MOD_NUMINPUTS21     8

#define INTYPENA    0
#define INTYPEMA    1
#define INTYPEV     2
#define INTYPERES   3

#include "../publicclass.h"
#include "../config.h"

class config_21
{
public:
    config_21();

    struct Bci
    {
        Bci_Main mainblk;                   // Основной блок (см. config.h)
        qint8 in_type[MOD_NUMINPUTS21];     // Тип входа №0…15 (0 - не исп., 1 - мА, 2 - В, 3 - резерв)
        float in_min[MOD_NUMINPUTS21];      // Минимальное значение диапазона изменения сигнала
        float in_max[MOD_NUMINPUTS21];      // Максимальное значение диапазона
        float in_vmin[MOD_NUMINPUTS21];     // Минимальное значение сигнала в инженерных единицах
        float in_vmax[MOD_NUMINPUTS21];     // Максимальное значение сигнала в инженерных единицах
        float setminmin[MOD_NUMINPUTS21];   // Значение аварийной уставки по минимальному значению
        float setmin[MOD_NUMINPUTS21];      // Значение предупредительной уставки по минимальному значению
        float setmax[MOD_NUMINPUTS21];      // Значение предупредительной уставки по максимальному значению
        float setmaxmax[MOD_NUMINPUTS21];	// Значение аварийной уставки по максимальному значению
        quint8 osc[MOD_NUMINPUTS21];        // Признаки источников запуска осциллограмм
        quint16 oscdly;                     // Задержка в мс начала фиксации максимумов
        float oscthr;                       // Пороговое значение, при превышении которого начинается запись осциллограмм
        float filterthr;                    // Пороговое значение, при превышении которого между замерами фильтр не работает
        float filtern;                      // Степень фильтрации (0 - отсутствует)
        float hysteresis;                   // Значение гистерезиса на сравнение с уставками (доля от диапазона)
    };

    Bci Bci_block, Bci_defblock;
    publicclass::DataRec Config[CONF_NUM+CONF_NUM21-1];
};

#endif // CONFIG21_H
