#ifndef TUNEKIV_H
#define TUNEKIV_H

#define M_BACBLOCKNUM 0

//#include "../tuneclass.h"
#include <QWidget>
#include <QtGlobal>

// class TuneKIV : public TuneClass
class TuneKIV
{
    //    Q_OBJECT
public:
    struct Bac
    { // Структура калибровочных параметров
        //записываются во флеш мезонинной платы:
        quint32 N1_TT[3]; // Число витков первичной обмотки
        float KmU[3]; // калибровочные коэффициенты по напряжению в 6 каналах
        float KmI1[3]; // калибровочные коэффициенты по току в 6 каналах для Кацп=1
        float KmI2[3]; // калибровочные коэффициенты по току в 6 каналах для Кацп=2
        float KmI4[3]; // калибровочные коэффициенты по току в 6 каналах для Кацп=4
        float KmI8[3]; // калибровочные коэффициенты по току в 6 каналах для Кацп=8
        float KmI16[3]; // калибровочные коэффициенты по току в 6 каналах для Кацп=16
        float KmI32[3]; // калибровочные коэффициенты по току в 6 каналах для Кацп=32
        float DPsi[6]; // коррекция фазы в i-м канале (в градусах)
        float Tmk0;    // Начальная температура МК для коррекции
        float TKUa[6]; //температурные коэффициенты линейной коррекции
        // по напряжениям и токам
        float TKUb[6]; //температурные коэффициенты квадратичной коррекции
        float TKPsi_a[3]; //температурные коэффициенты линейной коррекции по tg delta
        float TKPsi_b[3]; //температурные коэффициенты квадратичной коррекции по tg delta

        //записываются во флеш базовой платы
        float K_freq; // коррекция частоты
        float Art;    // коэффициент в канале Pt100, ед.АЦП/Ом
        float Brt;    // смещение в канале Pt100, ед.АЦП
    };

    struct Bda_in
    { // Структура блока выходных данных
        // в масштабах входных сигналов (для настройки)
        float Frequency;       // Частота в сети, Гц
        float IUefNat_filt[6]; // Истинные действующие значения сигналов (в вольтах или мА на входе)
        float IUeff_filtered[6]; // действующие значения сигналов по 1-й гармонике
        float phi_next_f[6]; // Углы сдвига сигналов по 1-й гармонике относительно Ua в градусах
        float Cbush[3]; // емкости вводов
        float Tg_d[3];  // tg delta вводов
        float Pt100_R;  // Измеренное сопротивление термометра, Ом
    };

    struct Bda
    {
        float Ueff_ADC[6];
        float Frequency;
        float Pt100;
    };

    struct RealDataStruct
    {
        float f[3];     // frequencies
        float u[3];     // voltages
        float i[3];     // currents
        float d[3];     // load phase
        float dpsiU[2]; // interphase voltages angles (B-A, C-B)
    };

    Bac m_Bac;
    Bda m_Bda;
    //    Bac m_Bac_block, m_Bac_newblock;
    //    Bda_in_struct m_Bda_in, m_Bda_block20, m_Bda_block60, m_Bda_blockMinus20;
    //    BdaStruct m_Bda_block, m_BdaPt100_80Om, m_BdaPt100_120Om;
    //    ValueModel *m_VModel;

    //    TuneKIV(int bacnum, S2ConfigType *, QObject *parent = nullptr);
    TuneKIV();
    QWidget *BacWidget();
    QWidget *BdaWidget();
    Bac defBacBlock();

    // private slots:
private:
    Bac m_defBacBlock;
    QWidget *m_BacWidget, *m_BdaWidget;
    bool m_BacWidgetIsSet, m_BdaWidgetIsSet;

    void SetDefCoefs();

public slots:
    void updateBacWidget();
    void updateBdaWidget();
    void updateFromBacWidget();
    void updateFromBdaWidget();
};

#endif // TUNEKIV_H
