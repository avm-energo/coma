#ifndef CHECKDIALOG80_H
#define CHECKDIALOG80_H

#include "abstractcheckdialog.h"

class CheckDialog80 : public AbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialog80(QWidget *parent = 0);

signals:

public slots:

private:

    struct Bd1
    {
        float Tmk;                  // 0 Температура кристалла микроконтроллера
        float Vbat;                 // 1 напряжение батареи, В
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
        float Harm[3][61];          // уровни гармоник от 2-й по 62-ю в % от 1-й для трех фаз
    };

    struct Bd6
    {
        float UefNatLin_filt[4]; 	// истинные действующие значения линейных напряжений трех фаз и их среднее
        float UeffLin_filtered[4];	// действующие значения 1-й гарм. линейных напряжений трех фаз и их среднее
    };

    struct Bd8
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

    struct Bip
    {
        quint8 ip[4];
    };

    Bd1 Bda_block1;
    Bd2 Bda_block2, Bda_block3;
    Bd4 Bda_block4, Bda_block5;
    Bd6 Bda_block6, Bda_block7;
    Bd8 Bda_block8;

    Bip Bip_block;
    QTimer *timer;
    QXlsx::Document *xlsx;
    bool XlsxWriting;
    int WRow;
    QTime *ElapsedTimeCounter;
    const QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: blue; font: bold 10px;}";

    void SetupUI();
    QWidget *AutoCheckUI(); // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля
    void RefreshAnalogValues(int bdnum); // обновление полей в GUI из полученного соответствующего Bd_block
    void PrepareHeadersForFile(int row); // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
};

#endif // CHECKDIALOG80_H
