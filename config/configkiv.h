#ifndef CONFIG84_H
#define CONFIGKIV_H

#define ID8084_START 1012
#define ID8084_COM 20

#include "../gen/modulebsi.h"
#include "config.h"

class ConfigKIV
{
public:
    explicit ConfigKIV(QVector<S2::DataRec> *config);

    typedef struct
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
    } Bci_Main;

    typedef struct
    {
        float Unom; //Номинальное линейное первичное напряжение в кВ
        // float Imax[3]; //Максимальные действующие значения токов вводов, мА
        float Umin; //Уставка контроля минимума напряжения(в % от номинального)
        float Imin;        //Уставка контроля минимума тока (в % от Imax)
        float C_init[3];   //Начальные значени емкостей вводов
        float Tg_init[3];  //Начальные значения tgδ вводов
        float corTg[3];    // коррекция  tg δ вводов
        float dC_pred;     // уставка предупредительной сигнализации
                           // по изменению емкости
        float dC_alarm;    // уставка аварийной сигнализации
                           // по изменению емкости
        float dTg_pred;    // уставка предупредительной сигнализации
                           // по изменению tg δ
        float dTg_alarm;   // уставка аварийной сигнализации
                           // по изменению tg δ
        float dIunb_pred;  // уставка предупредительной сигнализации
                           // по изменению небаланса токов
        float dIunb_alarm; // уставка аварийной сигнализации
                           // по изменению небаланса токов
        float GdC; // гистерезис на отключение сигнализации по dC, % от уставки
        float GdTg; // гистерезис на отключение сигнализации по dTg, % от уставки
        float GdIunb; // гистерезис на отключение сигнализации по небалансу токов
        float RTerm; // Сопротивление термометра при 0°С, Ом (только для МНК3)
        float W100; // Температурный коэффициент термометра (только для МНК3)
        float Iunb_init;    // Начальное действ. значение тока небаланса
        float Phy_unb_init; // Начальное значение угла тока небаланса
                            // относительно вектора тока фазы А
        quint32 NFiltr; // Интервал усреднения данных  (в периодах основной частоты)
        quint32 T_Data_Rec; // Интервал записи данных в ПЗУ (тренд), в секундах
        float LowU;
        quint8 IsU;
        quint8 IsIunb;
        quint32 Tevent_pred;
        quint32 Tevent_alarm;
        quint32 Trele_pred;
        quint32 Trele_alarm;
        float Unom_1;
        float Tg_pasp[3];
        float C_pasp[3];

    } Bci;

    typedef struct
    {
        quint8 IP[4];
        quint8 Mask[4];
        quint8 GateWay[4];
        quint16 Port[4];
        quint8 SNTP[4];
        quint32 baud;
        quint8 parity;
        quint8 stopbit;
        quint8 adrMB;
        quint8 isNTP;
    } Com;

    Bci_Main MainBlk; // Основной блок (см. config.h)
    Com Com_param;
    Bci Bci_block;

    void SetDefConf();
};

#endif // CONFIG84_H