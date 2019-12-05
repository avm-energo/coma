#ifndef CONFIG84_H
#define CONFIG84_H

#define ID8084_START    1012
#define ID8084_COM    20

#include "config.h"
#include "../gen/modulebsi.h"

class Config84
{
public:
    explicit Config84(QVector<S2::DataRec> &config);

    typedef struct
    {
        float Unom; //Номинальное линейное первичное напряжение в кВ
        float Imax[3]; //Максимальные действующие значения токов вводов, мА
        float Umin; //Уставка контроля минимума напряжения(в % от номинального)
        float Imin; //Уставка контроля минимума тока (в % от Imax)
        float C_init[3]; //Начальные значени емкостей вводов
        float Tg_init[3]; //Начальные значения tgδ вводов
        float corTg[3];	// коррекция  tg δ вводов
        float dC_pred;	// уставка предупредительной сигнализации
                    // по изменению емкости
        float dC_alarm;	// уставка аварийной сигнализации
                    // по изменению емкости
        float dTg_pred;	// уставка предупредительной сигнализации
                    // по изменению tg δ
        float dTg_alarm;   // уставка аварийной сигнализации
                       // по изменению tg δ
        float dIunb_pred;	// уставка предупредительной сигнализации
                        // по изменению небаланса токов
        float dIunb_alarm;   	// уставка аварийной сигнализации
                        // по изменению небаланса токов
        float GdC;		// гистерезис на отключение сигнализации по dC, % от уставки
        float GdTg;		// гистерезис на отключение сигнализации по dTg, % от уставки
        float GdIunb;	// гистерезис на отключение сигнализации по небалансу токов
        float RTerm;	// Сопротивление термометра при 0°С, Ом (только для МНК3)
        float W100;	// Температурный коэффициент термометра (только для МНК3)
        float Iunb_init;		// Начальное действ. значение тока небаланса
        float Phy_unb_init;	// Начальное значение угла тока небаланса
                        // относительно вектора тока фазы А
        quint32 NFiltr;	  // Интервал усреднения данных  (в периодах основной частоты)
        quint32 T_Data_Rec;  // Интервал записи данных в ПЗУ (тренд), в секундах
        float LowU;
        quint8 IsU;
        quint8 IsIunb;
        quint32 Tevent_pred;
        quint32 Tevent_alarm;
        quint32 Trele_pred;
        quint32 Trele_alarm;
        float Unom_1;           //номинальное первичное напряжение первой тройки
        float Tg_pasp[3];       //паспортные значения
        float C_pasp[3];

    } Bci;

    typedef struct
    {
        quint8		IP[4];
        quint8		Mask[4];
        quint8  	GateWay[4];
        quint16		Port[4];
        quint8		SNTP[4];
        quint32		baud;
        quint8		parity;
        quint8		stopbit;
        quint16     adrMB;
        quint8      isNTP;
    } Com;

    Com Com_param;
    Bci Bci_block;

    void SetDefConf();
};

#endif // CONFIG84_H
