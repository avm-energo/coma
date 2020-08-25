#ifndef CHECKKDV_H
#define CHECKKDV_H

#include <QWidget>

class CheckKDV
{
public:
    CheckKDV();

    typedef struct
    {
        float Frequency;       // Частота, Гц
        float IUefNat_filt[6]; // Истинные действующие значения сигналов(в вольтах
                               // или мА на входе)
        float UefNatLin_filt[3];
        float IUeff_filtered[6]; // Действующие значения сигналов по 1-ой гармонике
        float phi_next_f[6]; // Углы сдвига сигналов по 1-ой гармонике относительно
                             // Ua в градусах
        float PNatf[3]; // Истинная активная мощность по фазам
        float QNatf[3]; // Реактивная мощность по кажущейся полной истинной активной
        float SNatf[3]; // Кажущаяся полная мощность по эфф. токам и напряжениям
        float CosPhiNat[3]; // cos phi по истинной активной мощности, по фазам
        float Pf[3];        // активная мощность по 1-ой гарм. по фазам
        float Qf[3];        // реактивная мощность по 1-ой гарм.
        float Sf[3];        // полная мощность по 1-ой гарм.
        float CosPhi[3];    // cos phi по 1-ой гарм., по фазам
        float Pt100_R;      // Измеренное сопротивление термометра, Ом
    } Bda_in_struct;

    struct Bd2
    {
        float GrIUefNat_filt[4]; // истинные действующие значения сигналов трех фаз
                                 // и их среднее
        float GrIUeff_filtered[4]; // действующие значения сигналов трех фаз по 1-й
                                   // гармонике и их среднее
        float KrF[3];              // крест-фактор по трем фазам
    };

    struct Bd4
    {
        float U0;      // сигнал нулевой последовательности
        float U1;      // сигнал прямой последовательности
        float U2;      // сигнал обратной последовательности
        float Kunsim0; // коэфф. несимметрии по нулевой посл.
        float Kunsim2; // коэфф. несимметрии по обратной посл.
        float THD[3];  // общий коэфф. гармонических искажений трех фаз
    };

    struct Bd5
    {
        float Harm[3][61]; // уровни гармоник от 2-й по 62-ю в % от 1-й для трех фаз
    };

    struct Bd8
    {
        float UefNatLin_filt[4];   // истинные действующие значения линейных
                                   // напряжений трех фаз и их среднее
        float UeffLin_filtered[4]; // действующие значения 1-й гарм. линейных
                                   // напряжений трех фаз и их среднее
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

    typedef struct
    { // Ñòðóêòóðà áëîêà ðåñóðñíûõ ïàðàìåòðîâ
        float Vst;
        float Age;
        float Resurs;
        float reserv;
    } Bda_RESURS;
    //===============================================================================================
    typedef struct
    {                // Ñòðóêòóðà áëîêà âûõîäíûõ àðàìåòðîâ òåïëîâîé ìîäåëè
        float Tpred; // Âñå âðåìåíà - â ìèíóòàõ.
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
    } Bda_MODEL;

    typedef struct
    {
        float TempWinding;   // Температура ННТ обмотки (расчетная)
        float temperature;   // Температура окружающей среды
        float resistance;    // Сопротивление термометра, Ом
        float ExtTempWin[4]; // Температуры обмоток, измеренные встроенными
                             // датчиками(до 4шт.)
    } Bda_TEMP;

    Bda_in_struct Bda_block;
    Bd2 Bd2_Gr1, Bd3_Gr2;
    Bd4 Bd4_U0, Bd6_U0;
    Bd5 Bd5_UI_Harm, Bd7_UI_Harm;
    Bda_RESURS Bd9_Resurs;
    Bd10 Bd10_Power;
    Bda_MODEL Bd11_Model;
    Bda_TEMP Bd13_Temp;

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
    void FillBda_block(QWidget *parent);
    void FillBd2_Gr1(QWidget *parent);
    void FillBd3_Gr2(QWidget *parent);
    void FillBd4_U0(QWidget *parent);
    void FillBd5_UI_Harm(QWidget *parent);
    void FillBd6_U0(QWidget *parent);
    void FillBd7_UI_Harm(QWidget *parent);
    void FillBd9_Resurs(QWidget *parent);
    void FillBd10_Power(QWidget *parent);
    void FillBd11_Model(QWidget *parent);
    void FillBd13_Temp(QWidget *parent);

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
