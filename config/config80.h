#ifndef E_CONFIG_H
#define E_CONFIG_H

// Описание блока Bci
#define EBCI_EQTYPE      101
#define EBCI_NPOINTS     102
#define EBCI_NFILTR      103
#define EBCI_NHFILTR     104
#define EBCI_DDOSC       105
#define EBCI_UNOM1       106
#define EBCI_UNOM2       107
#define EBCI_INOM1       108
#define EBCI_INOM2       109
#define EBCI_DUOSC       110
#define EBCI_DIOSC       111
#define EBCI_DUIMIN      112
#define ECONF_NUM        23

#include "../publicclass.h"
#include "config.h"

class config_80
{
public:
    config_80();
    ~config_80();

    typedef struct
    {
        Bci_Main mainblk;       // Основной блок (см. config.h)
        quint32 eq_type;        // Тип контролируемого оборудования: 0 - 1фТАТ, 1 - 3фТАТ, 2 - 1фР, 3 - 3фР
        quint32 npoints;        // Количество точек оцифровки на период (64/80/128/256)
        quint32 nfiltr;         // Интервал усреднения данных (постоянная фильтрации)
        quint32 nhfiltr;        // Постоянная фильтрации гармоник
        quint32 ddosc;          // События-инициаторы запуска осциллографирования
        float unom1;            // Класс напряжения 1-й группы
        float unom2;            // Класс напряжения 2-й группы
        float inom1[6];         // Номинальный первичный ток внешнего ТТ по каналам
        float inom2[6];         // Номинальный вторичный ток внешнего ТТ по каналам
        float duosc;            // Уставка скачка напряжения для запуска осциллографирования (в % от номинального напряжения ТН)
        float diosc;            // Уставка скачка тока для запуска осциллографирования (в % от номинального тока ТТ)
        float duimin;           // Уставка контроля минимума сигналов (в %)
    } Bci;

    Bci Bci_block, Bci_defblock;
    publicclass::DataRec Config[ECONF_NUM];
};

#endif // E_CONFIG_H
