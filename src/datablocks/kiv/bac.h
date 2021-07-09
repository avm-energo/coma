#ifndef KIV_BAC_H
#define KIV_BAC_H

#include "../datablock.h"

class Bac : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
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

    Bac(QObject *parent = nullptr);

    void createWidget() override;
    void setDefBlock() override;
    void updateWidget() override;
    void updateFromWidget() override;
    BlockData *data();

private:
    std::unique_ptr<BlockData> m_blockData;
};

#endif // KIV_BAC_H
