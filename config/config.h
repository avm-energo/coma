#ifndef CONFIG
#define CONFIG

#include <QVector>
#include "../publicclass.h"

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
//#define CONF_NUM        10

#define DEF_ABS_104     205
#define DEF_CTYPE       2
#define DEF_CYCLE_104   5
#define DEF_T1_104      15
#define DEF_T2_104      10
#define DEF_T3_104      20
#define DEF_K_104       12
#define DEF_W_104       8

// определение базовых плат
#define MTB_00      0x00    // нет базовой платы
#define MTB_12      0x1200 // процессорный
#define MTB_21      0x2100 // аналоговый ввод DC
#define MTB_22      0x2200 // аналоговый ввод DC
#define MTB_31      0x3100 // дискретный ввод
#define MTB_35      0x3500 // дискретный вывод
#define MTB_80      0x8000 // аналоговый ввод AC, комбинируется с 8x
#define MTB_84      0x8400 // НКВВ, комбинируется с 8x
#define MTB_85      0x8500 // УСК, комбинируется с 8x
#define MTB_A1      0xA1 // ПКДН, прибор для Туркота
#define MTB_A2      0xA2 // МНК3

// определение мезонинов
#define MTM_00      0x00 // нет мезонина
#define MTM_21      0x0021 // аналоговый ввод
#define MTM_22      0x0022 // аналоговый ввод
#define MTM_31      0x0031 // дискретный ввод
#define MTM_35      0x0035 // дискретный вывод
#define MTM_81      0x0081 // 0U6I
#define MTM_82      0x0082 // 3U3I
#define MTM_83      0x0083 // 6U0I
#define MTM_85      0x0085 // 3U3I УСК (перегрузка по току 20х)

// определение файлов
#define FILE_CONF   1 // configuration
#define FILE_FW     3 // firmware
#define FILE_CJ     17 // commutation journal
#define FILE_EJ     18 // events journal (12->62)
#define FILE_OSC    1000 // oscilloscope info

class Config
{
public:

    explicit Config(QVector<publicclass::DataRec> &config);

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

    Bci_Main MainBlk;   // Основной блок (см. config.h)

    void SetDefBlock();
};


#endif // CONFIG

