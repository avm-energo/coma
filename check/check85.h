#ifndef CHECK85_H
#define CHECK85_H

#include <QWidget>

class Check_85
{
public:

    struct Bd1
    {
        float Frequency;            // Частота в сети
        float IUefNat_filt[9];  	// Истинные действующие значения сигналов (в вольтах или амперах на входе)
        float UefNatLin_filt[6];	// Истинные действующие значения линейных напряжений 1-й и 2-й групп
        float PNatf[3];             // истинная активная мощность по фазам
        float QNatf[3];             // реактивная мощность по кажущейся полной и истинной активной
        float SNatf[3];             // кажущаяся полная мощность по эфф. токам и напряжениям
        float CosPhiNat[3];         // cos phi по истинной активной мощности, по фазам
        quint32 DD_in;              // дискретные входные данные
                                    // разряды 0..11 -состояние входов 220_DI0.. 220_DI11
                                    // разряды 12,13 - состояние входов 24_DI0, 24_DI1
    };
    Bd1 Bd_block1;

    Check_85();
    QWidget *Bd1W(QWidget *parent);
    void FillBd1(QWidget *parent);

private:
    QString ValuesFormat, WidgetFormat;
};

#endif // CHECK85_H
