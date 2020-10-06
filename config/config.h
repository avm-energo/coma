#ifndef CONFIG
#define CONFIG

#include "../gen/s2.h"

#include <QMap>

#define BCI_MTYPEB 1
#define BCI_MTYPEM 2
#define BCI_CTYPE 3
#define BCI_ABS_104 4
#define BCI_CYCLE_104 5
#define BCI_T1_104 6
#define BCI_T2_104 7
#define BCI_T3_104 8
#define BCI_K_104 9
#define BCI_W_104 10

#define DEF_MTYPEB MTB_00
#define DEF_MTYPEM MTM_00
#define DEF_CTYPE 1
#define DEF_ABS_104 205
#define DEF_CYCLE_104 5
#define DEF_T1_104 15
#define DEF_T2_104 10
#define DEF_T3_104 20
#define DEF_K_104 12
#define DEF_W_104 8

// определение файлов
#define FILE_CONF 1   // configuration
#define FILE_FW 3     // firmware
#define FILE_CJ 17    // commutation journal
#define FILE_EJ 18    // events journal (12->62)
#define FILE_OSC 1000 // oscilloscope info

#define MTYPE_KTF 0xA287
#define MTYPE_KIV 0xA284
#define MTYPE_KDV 0xA387

class Config
{
public:
    explicit Config(QVector<S2::DataRec> *config, quint32 MTypeB, quint32 MTypeM);

    enum BaseBoards
    {
        MTB_00 = 0x00,
        MTB_12 = 0x12,
        MTB_21 = 0x21,
        MTB_22 = 0x22,
        MTB_31 = 0x31,
        MTB_35 = 0x35,
        MTB_80 = 0x80,
        MTB_81 = 0x81,
        MTB_84 = 0x84,
        MTB_85 = 0x85,
        MTB_87 = 0x87,
        MTB_A1 = 0xA1,
        MTB_A2 = 0xA2,
        MTB_A3 = 0xA3
    };

    BaseBoards Base;

    enum MezzanineBoards
    {
        MTM_00 = 0x00,
        MTM_21 = 0x21,
        MTM_22 = 0x22,
        MTM_31 = 0x31,
        MTM_35 = 0x35,
        MTM_81 = 0x81,
        MTM_82 = 0x82,
        MTM_83 = 0x83,
        MTM_84 = 0x84,
        MTM_85 = 0x85,
        MTM_86 = 0x86,
        MTM_87 = 0x87
    };

    struct Bci_Main
    {
        quint32 MTypeB;
        quint32 MTypeM;
        quint32 Ctype;   // Тип синхронизации времени от модуля Ц
        quint32 Abs_104; // Адрес базовой станции для протокола 104
        quint32 Cycle_104; // Интервал циклического опроса по протоколу МЭК 60870-5-104
        quint32 T1_104; // тайм-аут Т1 для протокола 104
        quint32 T2_104; // тайм-аут Т2 для протокола 104
        quint32 T3_104; // тайм-аут Т3 для протокола 104
        quint32 k_104;  // макс. кол-во неподтв. сообщений
        quint32 w_104;  // макс. кол-во сообщений, после которых необх. выдать
                        // подтверждение
    };

    Bci_Main MainBlk; // Основной блок (см. config.h)

    struct ModuleDesc
    {
        QString TextString;
        int Hex;
    };

    static const QMap<quint32, ModuleDesc> ModuleBaseBoards()
    {
        QMap<quint32, ModuleDesc> map;
        map[MTB_00] = { "Отсутствует", 0x00 };     // нет базовой платы
        map[MTB_12] = { "АВ-ТУК-12 (ЦКУ)", 0x12 }; // процессорный, не комбинируется
        map[MTB_21] = { "АВ-ТУК-21", 0x21 }; // аналоговый ввод DC, комбинируется с 2x,3x
        map[MTB_22] = { "АВ-ТУК-22", 0x22 }; // аналоговый ввод DC, комбинируется с 2x,3x
        map[MTB_31] = { "АВ-ТУК-31", 0x31 }; // дискретный ввод, комбинируется с 2x,3x
        map[MTB_35] = { "АВ-ТУК-35", 0x35 }; // дискретный вывод, комбинируется с 2x,3x
        map[MTB_80] = { "АВТУК-", 0x80 };    // аналоговый ввод AC, комбинируется с 81,82,83
        map[MTB_A2] = { "АВ-ТУК-81", 0x81 }; // МНК3, комбинируется с 84
        map[MTB_84] = { "АВ-ТУК-84", 0x84 }; // НКВВ, комбинируется с 81,82,83
        map[MTB_85] = { "АВ-ТУК-85", 0x85 }; // УСК, комбинируется с 85
        map[MTB_87] = { "АВ-ТУК-87 (ЧР)", 0x87 }; // ЧР, не комбинируется
        map[MTB_A1] = { "ПКС-1", 0xA1 }; // ПКДН, прибор для Туркота, не комбинируется
        map[MTB_A2] = { "АВМ", 0xA2 };   // МНК3, не комбинируется
        map[MTB_A3] = { "АВМ", 0xA3 };   // МНК3, не комбинируется
        return map;
    }

    static const QMap<quint32, ModuleDesc> ModuleMezzanineBoards()
    {
        QMap<quint32, ModuleDesc> map;
        map[MTM_00] = { "", 0x00 };           // нет мезонина
        map[MTM_21] = { "21", 0x21 };         // аналоговый ввод, комбинируется с 2x,3x
        map[MTM_22] = { "22", 0x22 };         // аналоговый ввод, комбинируется с 2x,3x
        map[MTM_31] = { "31", 0x31 };         // дискретный ввод, комбинируется с 2x,3x
        map[MTM_35] = { "35", 0x35 };         // дискретный вывод, комбинируется с 2x,3x
        map[MTM_81] = { "81 (Э2Т0Н)", 0x81 }; // 0U6I, комбинируется с 80,84
        map[MTM_82] = { "82 (Э1Т1Н)", 0x82 }; // 3U3I, комбинируется с 80,84
        map[MTM_83] = { "83 (Э0Т2Н)", 0x83 }; // 6U0I, комбинируется с 80,84
        map[MTM_84] = { "84", 0x84 };         // 3U3I, комбинируется с 80,81
        map[MTM_85] = { "85 (УСК)", 0x85 }; // 3U3I УСК (перегрузка по току 20х), комбинируется с 85
        map[MTM_87] = { "87", 0x87 };
        return map;
    }

    static const QMap<quint32, QString> ModuleBoards()
    {
        QMap<quint32, QString> Map;

        Map[MTB_A1] = "ПКС-1"; // ПКДН, прибор для Туркота, не комбинируется
        Map[MTYPE_KTF] = "АВМ-КТФ";
        Map[MTYPE_KIV] = "АВМ-КИВ";
        Map[MTYPE_KDV] = "АВМ-КДВ";
        return Map;
    }

    void SetDefBlock();
    // void Fill();
};

#endif // CONFIG
