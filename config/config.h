#ifndef CONFIG
#define CONFIG

#include "publicclass.h"

#define BCI_MTYPEB      1
#define BCI_MTYPEM      2
#define BCI_CTYPE       3
#define BCI_ABS_104     4
#define BCI_CYCLE_104   5
#define BCI_T1_104      6
#define BCI_T2_104      7
#define BCI_T3_104      8
#define BCI_K_104       9
#define BCI_W_104       10
#define CONF_NUM        11

#define DEF_ABS_104     205
#define DEF_CTYPE       2
#define DEF_CYCLE_104   5
#define DEF_T1_104      15
#define DEF_T2_104      10
#define DEF_T3_104      20
#define DEF_K_104       12
#define DEF_W_104       8

// определение базовых плат
#define MTB_12      0x12 // процессорный
#define MTB_21      0x21 // аналоговый ввод DC
#define MTB_22      0x22 // аналоговый ввод DC
#define MTB_31      0x31 // дискретный ввод
#define MTB_35      0x35 // дискретный вывод
#define MTB_80      0x80 // аналоговый ввод AC, комбинируется с 8x
#define MTB_81      0x81 // МНК3
#define MTB_84      0x84 // НКВВ, комбинируется с 8x
#define MTB_85      0x85 // УСК, комбинируется с 8x

// определение мезонинов
#define MTM_00      0x00 // нет мезонина
#define MTM_21      0x21 // аналоговый ввод
#define MTM_22      0x22 // аналоговый ввод
#define MTM_31      0x31 // дискретный ввод
#define MTM_35      0x35 // дискретный вывод
#define MTM_81      0x81 // 6U0I
#define MTM_82      0x82 // 3U3I
#define MTM_83      0x83 // 0U6I
#define MTM_85      0x85 // 3U3I УСК (перегрузка по току 20х)

// определение файлов
#define FILE_CONF   1 // configuration
#define FILE_FW     3 // firmware
#define FILE_CJ     17 // commutation journal
#define FILE_EJ     18 // events journal (12->62)
#define FILE_OSC    1000 // oscilloscope info

struct Bci_Main
{
    quint32 MTypeB;         // Тип базовой платы, для которой предназначена конфигурация
    quint32 MTypeM;         // Тип мезонина, для которого предназначена конфигурация
    quint32 Ctype;   		// Тип синхронизации времени от модуля Ц
    quint32 Abs_104;     	// Адрес базовой станции для протокола 104
    quint32 Cycle_104;      // Интервал циклического опроса по протоколу МЭК 60870-5-104
    quint32 T1_104;         // тайм-аут Т1 для протокола 104
    quint32 T2_104;         // тайм-аут Т2 для протокола 104
    quint32 T3_104;         // тайм-аут Т3 для протокола 104
    quint32 k_104;          // макс. кол-во неподтв. сообщений
    quint32 w_104;          // макс. кол-во сообщений, после которых необх. выдать подтверждение
};

struct Bhb_Main
{
    quint32 MType;          // тип модуля
    quint32 SerialNum;      // серийный номер платы
    quint32 HWVer;          // версия аппаратного обеспечения модуля (платы)
    quint32 ModSerialNum;   // серийный номер модуля целиком
};

#endif // CONFIG

