#ifndef CHECKVIBRKDV_H
#define CHECKVIBRKDV_H

#include <QWidget>

class CheckVibrKDV
{
public:
    CheckVibrKDV();

    struct Bd19
    {
        float VibrSCZ[6];
    };

    struct Bd20
    {
        float Vibr_SCZ_A[6];
        float Vibr_SCZ_V[6];
        float Vibr_SCZ_D[6];
        float Vibr_SCZ_A_temp[6];
        float Vibr_SCZ_V_temp[6];
        float Vibr_SCZ_D_temp[6];
    };

    Bd19 Bd_block19;
    Bd20 Bd_block20;

    void FillBd(QWidget *parent, QString Name, QString Value);
    void FillBd19(QWidget *parent);
    void FillBd20(QWidget *parent);


private:
    QString ValuesFormat, WidgetFormat;
};

#endif // CHECKVIBRKDV_H
