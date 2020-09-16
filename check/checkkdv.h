#ifndef CHECKKDV_H
#define CHECKKDV_H

#include <QWidget>

class CheckKDV
{
public:
    CheckKDV();

    typedef struct
    {
        float Pt100_R; // Измеренное сопротивление термометра, Ом
    } Bda_in_struct;

    struct Bd0
    {
        float Temp;
        float Vbat;
    };

    struct Bd_2_3
    {
        float IUefNat_filt[4]; // истинные действующие значения сигналов трех фаз
                               // и их среднее
        float IUeff_filtered[4]; // действующие значения сигналов трех фаз по 1-й
                                 // гармонике и их среднее
        float KrF[3];            // крест-фактор по трем фазам
    };

    struct Bd_4_6
    {
        float U0;      // сигнал нулевой последовательности
        float U1;      // сигнал прямой последовательности
        float U2;      // сигнал обратной последовательности
        float Kunsim0; // коэфф. несимметрии по нулевой посл.
        float Kunsim2; // коэфф. несимметрии по обратной посл.
        float THD[3];  // общий коэфф. гармонических искажений трех фаз
    };

    struct Bd8
    {
        float UefNatLin_filt[4];   // истинные действующие значения линейных
                                   // напряжений трех фаз и их среднее
        float UeffLin_filtered[4]; // действующие значения 1-й гарм. линейных
                                   // напряжений трех фаз и их среднее
    };

    struct Bd9
    {
        float Vst;
        float Age;
        float Resurs;
        float MotHnorm;
        float MotHover;
        float MotHTotal;
        float reserv;
    };

    struct Bd10
    {
        float PNatf[4]; // истинная активная мощность по фазам и суммарная
        float QNatf[4]; // реактивная мощность по кажущейся полной и истинной активной
        float SNatf[4]; // кажущаяся полная мощность по эфф. токам и напряжениям
        float CosPhiNat[4]; // cos phi по истинной активной мощности, по фазам и
                            // средний
        float Pf[4];     // активная мощность по 1-й гарм. по фазам и суммарная
        float Qf[4];     // реактивная мощность по 1-й гарм.
        float Sf[4];     // полная мощность по по 1-й гарм.
        float CosPhi[4]; // cos phi по 1-й гарм., по фазам и средний
    };

    struct Bd11
    {
        float Tpred;
        float Tdop;
        float Tdop105;
        float Tdop110;
        float Tdop115;
        float Tdop120;
        float Tdop125;
        float Tdop130;
        float Tdop135;
        float Tdop140;
        float Tdop145;
        float Tdop150;
        float Tdop155;
        float Tdop160;
        float Tdop165;
        float Tdop170;
        float Tdop175;
        float Tdop180;
        float Tdop185;
        float Tdop190;
        float Tdop195;
        float Tdop200;
    };

    struct Bd13
    {
        float TempWinding;
        float temperature;
        float resistance;
        float ExtTempWin[4];
        quint32 res;
    };

    struct Bd17
    {
        float Frequency;
        float phi_next_f[6];
    };

    struct Bd18
    {
        float Istart;
        quint32 IsTime;
    };

    Bda_in_struct Bda_block;
    Bd0 Bd_block0;
    Bd_2_3 Bd_block2, Bd_block3;
    Bd_4_6 Bd_block4, Bd_block6;
    Bd8 Bd_block8;
    Bd9 Bd_block9;
    Bd10 Bd_block10;
    Bd11 Bd_block11;
    Bd13 Bd_block13;
    Bd17 Bd_block17;
    Bd18 Bd_block18;

    // Check_KDV();
    QWidget *Bd1W(QWidget *parent);
    QWidget *Bd2W(QWidget *parent);
    QWidget *Bd3W(QWidget *parent);
    QWidget *Bd4W(QWidget *parent);
    QWidget *Bd5W(QWidget *parent);
    QWidget *Bd6W(QWidget *parent);
    QWidget *Bd7W(QWidget *parent);
    QWidget *Bd8W(QWidget *parent);
    void FillBd(QWidget *parent, QString Name, QString Value);
    void FillBd0(QWidget *parent);
    void FillBd2(QWidget *parent);
    void FillBd3(QWidget *parent);
    void FillBd4(QWidget *parent);

    void FillBd6(QWidget *parent);
    void FillBd8(QWidget *parent);
    void FillBd9(QWidget *parent);
    void FillBd10(QWidget *parent);
    void FillBd11(QWidget *parent);
    void FillBd13(QWidget *parent);
    void FillBd17(QWidget *parent);
    void FillBd18(QWidget *parent);

    QWidget *BdUI(int bdnum);
    QWidget *CustomTab();

    void RefreshAnalogValues(int bdnum);
    void PrepareHeadersForFile(int row);
    void WriteToFile(int row, int bdnum);
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
    void PrepareAnalogMeasurements();

private:
    QString ValuesFormat, WidgetFormat;
};

#endif // CHECKKDV_H
