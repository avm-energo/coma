#ifndef CONFIGKIV_H
#define CONFIGKIV_H

#define ID8084_START 1012
#define ID8084_COM 20

#include "config.h"

#include <QStack>

class ConfigKIV
{
public:
    explicit ConfigKIV(S2ConfigType *config);

    typedef struct
    {
        float Unom; //Номинальное линейное первичное напряжение в кВ

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
        float Unom_1;
        float Tg_pasp[3];
        float C_pasp[3];

    } Bci;

    Bci Bci_block;

    //    QStack<Bci> m_BciStack;

    void setDefConf();
    //    Error::Msg getAndPushConfig();
    //    Bci popConfig();
    //    Error::Msg popAndWriteConfig();
    S2ConfigType *S2Config();
    void setS2Config(S2ConfigType *s2config);

private:
    S2ConfigType *m_S2Config;

    void setDefConf(Bci &bci);
};

#endif // CONFIGKIV_H
