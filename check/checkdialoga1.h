#ifndef CHECKDIALOGA1_H
#define CHECKDIALOGA1_H

#include "abstractcheckdialog.h"

class CheckDialogA1 : public AbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialogA1(QWidget *parent = 0);

signals:

public slots:

private:


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

    QWidget *AutoCheckUI(); // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля
    void RefreshAnalogValues(int bdnum); // обновление полей в GUI из полученного соответствующего Bd_block
    void PrepareHeadersForFile(int row); // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
};

#endif // CHECKDIALOGA1_H
