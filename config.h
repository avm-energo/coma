#ifndef CONFIG_H
#define CONFIG_H

// Обмен с модулями
#define ResOk   0x11 // ответ "всё в порядке"
#define GBsi    0x21 // запрос блока стартовой информации
#define Gac     0x22 // запрос настроечных коэффициентов
#define Gda     0x23 // запрос текущих данных без настройки
#define GBd     0x24 // запрос блока текущих данных
#define GF      0x25 // запрос файла конфигурации
#define GNosc   0x26 // запрос информации об осциллограммах
#define GBosc   0x27 // запрос осциллограммы
#define GBev    0x28 // запрос банка событий
#define Wac     0x31 // запись настроечных коэффициентов
#define WF      0x32 // запись файла конфигурации
#define Start   0x3e // начало посылки
#define MStart  0x3c // начало посылки модуля
#define LN      0x41 // зажечь светодиод
#define LF      0x42 // погасить светодиод
#define IP      0x43 // запрос IP-адреса
#define PS      0x44 // контроль 1PPS
#define SegOk   0x55 // ответ "сегмент принят в порядке"
#define Err     0xF0 // ответ "ошибка"

#endif // CONFIG_H

