#ifndef CHECK80_H
#define CHECK80_H

#include <QWidget>

#define C80_MAXHARMINDEX    61
#define C80_BDA_IN  1
#define C80_BD_31   2
#define C80_BD_32   3
#define C80_BDNS_31 4
#define C80_BDH_31  5
#define C80_BDNS_32 6
#define C80_BDH_32  7
#define C80_BDL_31  8
#define C80_BDL_32  9
#define C80_BDPWR   10

class Check_80
{
public:

    struct Bd1
    {
        float Frequency;            // 2 частота сигналов, Гц,
        float IUefNat_filt[6];      // 3-8 Истинные действующие значения сигналов в вольтах и амперах
        float IUeff_filtered[6];    // 9-14 Действующие значения сигналов по 1-й гармонике
        float phi_next_f[6];        // 15-20 Углы сдвига между сигналами по 1-й гармонике в 6 каналах в градусах,
        float PNatf[3];             // 21-23 Истинная активная мощность, по фазам
        float SNatf[3];             // 24-26 Кажущаяся полная мощность по эффективным  токам и напряжениям,
        float QNatf[3];             // 27-29 Реактивная мощность по кажущейся полной и истинной активной
        float CosPhiNat[3];         // 30-32 cos phi по истинной активной мощности
        float Pf[3];                // 33-35 Активная мощность по 1-й гармонике
        float Qf[3];                // 36-38 Реактивная мощность по 1-й гармонике
        float Sf[3];                // 39-41 Полная мощность по 1-й гармонике
        float CosPhi[3];            // 42-44 cos phi по 1-й гармонике
    };

    // Первая тройка: для модулей 81 первые 8 сигналов – токи в А, для модулей 82 и 83 – напряжения в кВ.
    // Вторая тройка: для модулей 81 и 82 первые 8 сигналов – токи в А, для модулей 83 – напряжения в кВ.
    struct Bd2
    {
        float IUefNat_filt[4];      // истинные действующие значения сигналов трех фаз и их среднее
        float IUeff_filtered[4];    // действующие значения сигналов трех фаз по 1-й гармонике и их среднее
        float KrF[3];               // крест-фактор по трем фазам
    };

    struct Bd4
    {
        float U0;                   // сигнал нулевой последовательности
        float U1;                   // сигнал прямой последовательности
        float U2;                   // сигнал обратной последовательности
        float Kunsim0;              // коэфф. несимметрии по нулевой посл.
        float Kunsim2;              // коэфф. несимметрии по обратной посл.
        float THD[3];               // общий коэфф. гармонических искажений трех фаз
    };

    struct Bd5
    {
        float Harm[3][61];          // уровни гармоник от 2-й по 62-ю в % от 1-й для трех фаз
    };

    struct Bd8
    {
        float UefNatLin_filt[4]; 	// истинные действующие значения линейных напряжений трех фаз и их среднее
        float UeffLin_filtered[4];	// действующие значения 1-й гарм. линейных напряжений трех фаз и их среднее
    };

    struct Bd10
    {
        float PNatf[4];		// истинная активная мощность по фазам и суммарная
        float QNatf[4];		// реактивная мощность по кажущейся полной и истинной активной
        float SNatf[4];		// кажущаяся полная мощность по эфф. токам и напряжениям
        float CosPhiNat[4];	// cos phi по истинной активной мощности, по фазам и средний
        float Pf[4];		// активная мощность по 1-й гарм. по фазам и суммарная
        float Qf[4];		// реактивная мощность по 1-й гарм.
        float Sf[4];		// полная мощность по по 1-й гарм.
        float CosPhi[4];	// cos phi по 1-й гарм., по фазам и средний
    };

    Bd1 Bd_block1;
    Bd2 Bd_block2, Bd_block3;
    Bd4 Bd_block4, Bd_block6;
    Bd5 Bd_block5, Bd_block7;
    Bd8 Bd_block8, Bd_block9;
    Bd10 Bd_block10;

    Check_80();
    QWidget *Bd1W(QWidget *parent);
    QWidget *Bd2W(const QString &title, const QString &begin, QWidget *parent);
    QWidget *Bd4W(const QString &title, const QString &begin, QWidget *parent);
    QWidget *Bd5W(const QString &title, const QString &begin, QWidget *parent);
    QWidget *Bd8W(const QString &title, const QString &begin, QWidget *parent);
    QWidget *Bd10W(QWidget *parent);
    QWidget *Bd_1W(QWidget *parent);
    QWidget *Bd_2W(QWidget *parent);
    QWidget *Bd_3W(QWidget *parent);
    QWidget *Bd_4W(QWidget *parent);
    QWidget *Bd_5W(int num, QWidget *parent);
    QWidget *Bd_6W(QWidget *parent);
    QWidget *Bd_7W(int num, QWidget *parent);
    QWidget *Bd_8W(QWidget *parent);
    QWidget *Bd_9W(QWidget *parent);
    QWidget *Bd_10W(QWidget *parent);
    void FillBd2W(const QString &begin, Bd2 &Bd, QWidget *parent);
    void FillBd4W(const QString &begin, Bd4 &Bd, QWidget *parent);
    void FillBd5W(const QString &begin, Bd5 &Bd, QWidget *parent);
    void FillBd8W(const QString &begin, Bd8 &Bd, QWidget *parent);
    void FillBd1(QWidget *parent);
    void FillBd2(QWidget *parent);
    void FillBd3(QWidget *parent);
    void FillBd4(QWidget *parent);
    void FillBd5(QWidget *parent);
    void FillBd6(QWidget *parent);
    void FillBd7(QWidget *parent);
    void FillBd8(QWidget *parent);
    void FillBd9(QWidget *parent);
    void FillBd10(QWidget *parent);

private:
    QString ValuesFormat, WidgetFormat;
};

#endif // CHECK80_H
