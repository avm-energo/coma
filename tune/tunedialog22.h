#ifndef TUNEDIALOG22_H
#define TUNEDIALOG22_H

#include <QDialog>
#include "eabstracttunedialog.h"
#include "../check/check22.h"

#define ATUNENUMPOINTS  1 // количество точек, по которым выдаются значения в блоке Bda
#define TUNEFILELENGTH  256

#define TTUNE_0     1
#define TTUNE_W100  2

class TuneDialog22 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialog22(int type = BT_BASE, QWidget *parent = 0);

signals:

public slots:

private:
    Check22::Bda Bda0, BdaW100;

    struct Bac
    {
        float fb;
        float fk;
    };

    Bac Bac_block[AIN22_NUMCH];

    int BoardType;
    int ChNum;
    Config22 *C22;

    void SetupUI();
    void SetLbls();
    void SetPf();
    void FillBac();
    void FillBackBac();
    void GetBdAndFillMTT();
    int ShowScheme();
    int Show0();
    int ShowW100();

    int Tune();
    int TuneChannel(int Type);
    void ShowErrMsg(int);
    bool CalcNewTuneCoef();
    bool CheckAndShowTune0();
    bool CheckAndShowTuneW100();
    bool CheckTuneCoefs();

private slots:
    void SetDefCoefs();
    int ReadAnalogMeasurements();
    void TuneOneChannel();
};

#endif // TUNEDIALOG22_H
