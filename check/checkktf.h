#ifndef CHECKKTF_H
#define CHECKKTF_H

#include <QWidget>
#include "../gen/maindef.h"


class CheckKTF
{
public:
    explicit CheckKTF();
    ~CheckKTF();

    struct Bd0
    {
        float Temp;
        float Vbat;
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

    struct Bd2
    {
        float Frequency;
        float IUefNat_filt[4];
        float IUeff_filtered[4];
        float KrF[3];
        float U0;
        float U1;
        float U2;
        float Kunsim0;
        float Kunsim2;
        float THD[3];
        float UI_Harm[3][61];
    };
    struct Bd3
    {
        float Frequency;
        float IUefNat_filt[4];    
        float IUeff_filtered[4];
        float KrF[3];
        float U0;
        float U1;
        float U2;
        float Kunsim0;
        float Kunsim2;
        float THD[3];
        float UI_Harm[3][61];
    };

    struct Bd10
    {
        float PNatf[4];
        float QNatf[4];
        float SNatf[4];
        float CosPhiNat[4];
        float Pf[4];
        float Qf[4];
        float Sf[4];
        float CosPhi;
    };

    struct Bd4
    {
        float U0;
        float U1;
        float U2;
        float Kunsim0;
        float Kunsim2;
        float THD[3];

    };

    struct Bd8
    {
        float UefNatLin_filt[4];
        float UeffLin_filtered[4];
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

    struct Bd9
    {
        float Vst;
        float Age;
        float Resurs;
        float reserv;
    };
    struct Bd18
    {
        float Istart;

    };



    Bd0 Bd_block0;
    Bd13 Bd_block13;
    Bd17 Bd_block17;
    Bd2 Bd_block2;
    Bd3 Bd_block3;
    Bd10 Bd_block10;
    Bd4 Bd_block4;
    Bd8 Bd_block8;
    Bd11 Bd_block11;
    Bd9 Bd_block9;
    Bd18 Bd_block18;



    QWidget *Bd1W(QWidget *parent);
    QWidget *Bd2W(QWidget *parent);
    QWidget *Bd3W(QWidget *parent);
    QWidget *Bd4W(QWidget *parent);
    QWidget *Bd5W(QWidget *parent);
    QWidget *Bd6W(QWidget *parent);
    QWidget *Bd7W(QWidget *parent);
    QWidget *Bd8W(QWidget *parent);
    QWidget *Bd9W(QWidget *parent);
    QWidget *Bd10W(QWidget *parent);
    QWidget *Bd11W(QWidget *parent);
    QWidget *Bd12W(QWidget *parent);
    QWidget *Bd13W(QWidget *parent);
    QWidget *Bd14W(QWidget *parent);
    QWidget *Bd15W(QWidget *parent);
    QWidget *Bd16W(QWidget *parent);
    QWidget *Bd17W(QWidget *parent);
    QWidget *Bd18W(QWidget *parent);

    void FillBd(QWidget *parent, QString Name, QString Value);
    void FillBd0(QWidget *parent);
    void FillBd13(QWidget *parent);
    void FillBd17(QWidget *parent);
    void FillBd2(QWidget *parent);
    void FillBd3(QWidget *parent);
    void FillBd10(QWidget *parent);
    void FillBd8(QWidget *parent);
    void FillBd11(QWidget *parent);
    void FillBd9(QWidget *parent);
    void FillBd18(QWidget *parent);



    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля
    QWidget *CustomTab();
    void RefreshAnalogValues(int bdnum); // обновление полей в GUI из полученного соответствующего Bd_block
    void PrepareHeadersForFile(int row); // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
    void PrepareAnalogMeasurements();

private:
    QString ValuesFormat, WidgetFormat;
float QQQ=0;

};

#endif // CHECKKTF_H
