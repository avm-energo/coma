#ifndef TUNEDIALOG22_H
#define TUNEDIALOG22_H

#include <QDialog>
#include "eabstracttunedialog.h"
#include "../check/check22.h"
#include "../gen/modulebsi.h"

#define ATUNENUMPOINTS  1 // количество точек, по которым выдаются значения в блоке Bda
#define TUNEFILELENGTH  256

#define TTUNE_0     1
#define TTUNE_W100  2

#define COEFSNUM    2

class QGroupBox;

class TuneDialog22 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialog22(BoardTypes type = BoardTypes::BT_BASE, QWidget *parent = nullptr);

signals:

public slots:

private:
    Check22::Bda Bda0, BdaW100;

    struct Bac
    {
        float fbin[AIN22_NUMCH];
        float fkin[AIN22_NUMCH];
    };

    struct BacRCoef
    {
        float RzeroT[COEFSNUM] = {100, 1000};
    };

    BacRCoef Rcoef;

    Bac Bac_block[COEFSNUM];

    int mode;
    int BoardType;
    int ChNum, CoefNum;
    Config22 *C22;
    const float R0[COEFSNUM] = {100, 1000};

    void SetupUI();
    QGroupBox *CoeffGB(const QString &title, const QString &coeff);
#if PROGSIZE != PROGSIZE_EMUL
    void SetLbls();
    void SetPf();
    int ShowScheme();
    int Show0(int coef);
    int ShowW100(int coef);

    int Tune();
    int TuneChannel(int Type);
    void ShowErrMsg(int);
    bool CalcNewTuneCoef();
    bool CheckAndShowTune0();
    bool CheckAndShowTuneW100();
    bool CheckTuneCoefs();
    void GetBdAndFillMTT();
    int TuneOneChannelFunc();
#endif
    
    void FillBac(int bacnum);
    void FillBackBac(int bacnum);

private slots:
    void SetDefCoefs();
#if PROGSIZE != PROGSIZE_EMUL
    int ReadAnalogMeasurements();
    int TuneOneChannel();
#endif
};

#endif // TUNEDIALOG22_H
