#ifndef CONFIGKIV_H
#define CONFIGKIV_H

#define ID8084_START 1012
#define ID8084_COM 20

#include "config.h"
#include "configkxx.h"

#include <QStack>

class ConfigKIV
{
public:
    explicit ConfigKIV();

    struct Bci
    {
        //Номинальное линейное первичное напряжение в кВ
        float Unom;
        //Уставка контроля минимума напряжения(в % от номинального)
        float Umin;
        //Уставка контроля минимума тока (в % от Imax)
        float Imin;
        //Начальные значени емкостей вводов
        float C_init[3];
        //Начальные значения tgδ вводов
        float Tg_init[3];
        // коррекция  tg δ вводов
        float corTg[3];
        // уставка предупредительной сигнализации по изменению емкости
        float dC_pred;
        // уставка аварийной сигнализации по изменению емкости
        float dC_alarm;
        // уставка предупредительной сигнализации по изменению tg δ
        float dTg_pred;
        // уставка аварийной сигнализации по изменению tg δ
        float dTg_alarm;
        // уставка предупредительной сигнализации по изменению небаланса токов
        float dIunb_pred;
        // уставка аварийной сигнализации по изменению небаланса токов
        float dIunb_alarm;
        // гистерезис на отключение сигнализации по dC, % от уставки
        float GdC;
        // гистерезис на отключение сигнализации по dTg, % от уставки
        float GdTg;
        // гистерезис на отключение сигнализации по небалансу токов
        float GdIunb;
        // Сопротивление термометра при 0°С, Ом (только для МНК3)
        float RTerm;
        // Температурный коэффициент термометра (только для МНК3)
        float W100;
        // Начальное действ. значение тока небаланса
        float Iunb_init;
        // Начальное значение угла тока небаланса относительно вектора тока фазы А
        float Phy_unb_init;
        // Интервал усреднения данных  (в периодах основной частоты)
        quint32 NFiltr;
        // Интервал записи данных в ПЗУ (тренд), в секундах
        quint32 T_Data_Rec;
        float LowU;
        quint8 IsU;
        quint8 IsIunb;
        quint32 Tevent_pred;
        quint32 Tevent_alarm;
        float Unom_1;
        float Tg_pasp[3];
        float C_pasp[3];
    };

    Bci Bci_block;

    //    QStack<Bci> m_BciStack;

    void setDefConf();
    //    Error::Msg getAndPushConfig();
    //    Bci popConfig();
    //    Error::Msg popAndWriteConfig();
    //    S2DataTypes::S2ConfigType *S2Config();
    //    void setS2Config(S2DataTypes::S2ConfigType *s2config);
    Config *MainConfig();
    ConfigKxx *KxxConfig();

private:
    Config *m_mainConfig;
    ConfigKxx *m_KxxConfig;

    S2DataTypes::S2ConfigType *m_S2Config;
};

#endif // CONFIGKIV_H
