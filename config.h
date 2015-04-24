#ifndef CONFIG_H
#define CONFIG_H

// Обмен с модулями
#define CN_ResOk   0x11 // ответ "всё в порядке"
#define CN_GBsi    0x21 // запрос блока стартовой информации
#define CN_Gac     0x22 // запрос настроечных коэффициентов
#define CN_Gda     0x23 // запрос текущих данных без настройки
#define CN_GBd     0x24 // запрос блока текущих данных
#define CN_GF      0x25 // запрос файла конфигурации
#define CN_GNosc   0x26 // запрос информации об осциллограммах
#define CN_GBosc   0x27 // запрос осциллограммы
#define CN_GBev    0x28 // запрос банка событий
#define CN_Wac     0x31 // запись настроечных коэффициентов
#define CN_WF      0x32 // запись файла конфигурации
#define CN_Start   0x3e // начало посылки
#define CN_MStart  0x3c // начало посылки модуля
#define CN_Cln     0x41 // зажечь светодиод
#define CN_Clf     0x42 // погасить светодиод
#define CN_Gip     0x43 // запрос IP-адреса
#define CN_Cps     0x44 // контроль 1PPS
#define CN_Cnc     0x51 // команда перехода на новую конфигурацию
#define CN_SegOk   0x55 // ответ "сегмент принят в порядке"
#define CN_ResErr  0x0f // ответ "ошибка"

#endif // CONFIG_H

