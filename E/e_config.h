#ifndef E_CONFIG_H
#define E_CONFIG_H

// Описание блока Bci
#define EBCI_MTYPE       1
#define EBCI_MTYPE1      2
#define EBCI_EQTYPE      3
#define EBCI_NPOINTS     4
#define EBCI_NFILTR      5
#define EBCI_NHFILTR     6
#define EBCI_DDOSC       7
#define EBCI_UNOM1       8
#define EBCI_UNOM2       9
#define EBCI_INOM1       10
#define EBCI_INOM2       11
#define EBCI_DUOSC       12
#define EBCI_DIOSC       13
#define EBCI_DUIMIN      14
#define EBCI_CTYPE       15
#define EBCI_ABS_104     16
#define EBCI_CYCLE_104   17
#define EBCI_T1_104      18
#define EBCI_T2_104      19
#define EBCI_T3_104      20
#define EBCI_K_104       21
#define EBCI_W_104       22
#define ECONF_NUM        23

#include "../publicclass.h"

class e_config
{
public:
    e_config();
    ~e_config();

    typedef struct
    {
        quint32 MType;          // Тип модуля, для которого предназначена конфигурация
        quint32 MType1;         // Подтип модуля, для которого предназначена конфигурация
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
        quint32 Ctype;   		// Тип синхронизации времени от модуля Ц
        quint32 Abs_104;     	// Адрес базовой станции для протокола 104
        quint32 Cycle_104;      // Интервал циклического опроса по протоколу МЭК 60870-5-104
        quint32 T1_104;         // тайм-аут Т1 для протокола 104
        quint32 T2_104;         // тайм-аут Т2 для протокола 104
        quint32 T3_104;         // тайм-аут Т3 для протокола 104
        quint32 k_104;          // макс. кол-во неподтв. сообщений
        quint32 w_104;          // макс. кол-во сообщений, после которых необх. выдать подтверждение
    } Bci;

    Bci Bci_block, Bci_defblock;
    publicclass::DataRec Config[ECONF_NUM];
};

#endif // E_CONFIG_H
