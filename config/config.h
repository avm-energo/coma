#ifndef CONFIG
#define CONFIG

#include "../gen/datatypes.h"

#include <QMap>

constexpr int BCI_MTYPEB = 1;
constexpr int BCI_MTYPEM = 2;
constexpr int BCI_CTYPE = 3;
constexpr int BCI_ABS_104 = 4;
constexpr int BCI_CYCLE_104 = 5;
constexpr int BCI_T1_104 = 6;
constexpr int BCI_T2_104 = 7;
constexpr int BCI_T3_104 = 8;
constexpr int BCI_K_104 = 9;
constexpr int BCI_W_104 = 10;

constexpr int DEF_CTYPE = 1;
constexpr int DEF_ABS_104 = 205;
constexpr int DEF_CYCLE_104 = 5;
constexpr int DEF_T1_104 = 15;
constexpr int DEF_T2_104 = 10;
constexpr int DEF_T3_104 = 20;
constexpr int DEF_K_104 = 12;
constexpr int DEF_W_104 = 8;

class Config
{
public:
    explicit Config(S2DataTypes::S2ConfigType *config);

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

    void setConfig(S2DataTypes::S2ConfigType *config);
    void SetDefConf();
    QWidget *MainWidget(QWidget *parent);
    QWidget *TimeWidget(QWidget *parent);
    void Fill();
    void FillBack();

    static void removeFotter(S2DataTypes::S2ConfigType *config);

private:
    QObject *ParentMainbl, *ParentCtype;
};

#endif // CONFIG
