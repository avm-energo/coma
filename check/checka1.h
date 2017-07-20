#ifndef CHECKA1_H
#define CHECKA1_H

#include <QWidget>

class Check_A1
{
public:

    // Блок Bdа_in – входная информация о переменных напряжениях в реальном масштабе на входах модуля
    // Блок Bdа_out  - выходные сигналы переменного тока в первичном масштабе
    struct A1_Bd1
    {
        float UefNat_filt[2];	// Истинные действующие значения напряжений (в вольтах  на входе прибора)
        float Uef_filt[2];		// Действующие значения первых гармоник напряжений (в вольтах  на входе прибора)
        float Phy;              // разность фаз первых гармоник напряжений
        float Frequency;		// Частота в сети
    };

    // Блок Bdа_h –гармонический состав напряжений в % от первой гармоники
    struct A1_Bd2
    {
        float HarmBuf[2][61]; // 1-й индекс – номер канала, 2-й – номер гармоники (в 0-й ячейке – 2-я гармоника, в 60-й – 62я)
    };

    // Блок Bdа_in_an  - входные сигналы постоянного тока
    struct A1_Bd3
    {
        float Pt100_R;	// Измеренное сопротивление, Ом
        float EXTmA1_I; // Ток в канале EXTmA1, мА
        float EXTmA2_I; // Ток в канале EXTmA2, мА
    };

    // Блок Bdа_out_an  - выходные сигналы постоянного тока
    struct A1_Bd4
    {
        float Tmk;	// Температура кристалла микроконтроллера, °С
        float Vbat;	// Напряжение аккумуляторной батареи, В
        float Tamb;	// Температура окружающей среды, °С
        float Hamb;	// Относительная влажность окружающей среды, %
    };

    // Блок Bdа – оцифрованные сигналы в масштабах АЦП и частота в Гц
    struct Bda
    {
        float Ueff_ADC[2];          // 0-1 Действующие значения напряжений в единицах АЦП
        float Frequency;            // 2 частота сигналов, Гц,
        float Pt100;                // 3 значение температурного сигнала
        float EXTmA1;               // 4 значение первого сигнала (4-20) мА
        float EXTmA2;               // 5 значение второго сигнала (4-20) мА
    };

    Bda Bda_block;
    A1_Bd1 Bda_in, Bda_out;
    A1_Bd2 Bda_h;
    A1_Bd3 Bda_in_an;
    A1_Bd4 Bda_out_an;

    Check_A1();
    QWidget *BdaW(QWidget *parent);
    QWidget *Bd1W(const QString &begin, QWidget *parent);
    QWidget *Bd2W(const QString &begin, QWidget *parent);
    QWidget *Bd3W(const QString &begin, QWidget *parent);
    QWidget *Bd4W(const QString &begin, QWidget *parent);
    QWidget *Bda_inW(QWidget *parent);
    QWidget *Bda_outW(QWidget *parent);
    QWidget *Bda_hW(QWidget *parent);
    QWidget *Bda_in_anW(QWidget *parent);
    QWidget *Bda_out_anW(QWidget *parent);
    void FillBda(QWidget *parent);
    void FillBd1W(const QString &begin, A1_Bd1 Bda, QWidget *parent);
    void FillBda_in(QWidget *parent);
    void FillBda_out(QWidget *parent);
    void FillBda_h(QWidget *parent);
    void FillBda_in_an(QWidget *parent);
    void FillBda_out_an(QWidget *parent);
};

#endif // CHECKA1_H
