#ifndef TUNEDIALOG21_H
#define TUNEDIALOG21_H

#include <QDialog>
#include "eabstracttunedialog.h"
#include "../config/config21.h"
#include "../check/check21.h"
#include "../gen/modulebsi.h"
#include "../gen/maindef.h"

#define TUNEFILELENGTH  256

#define ATUNE_U0    0
#define ATUNE_I20   1
#define ATUNE_U5    2

class TuneDialog21 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialog21(BoardTypes type = BoardTypes::BT_BASE, QWidget *parent = nullptr);
    ~TuneDialog21();

signals:

public slots:

private:
    Check21::Bda BdaI0, BdaU0, Bda5, Bda20, BdaI0copy, BdaU0copy, Bda5copy, Bda20copy;

    struct Bac_Old
    {
        float fbin_I;
        float fkuin;
        float fkiin;
    };

    struct Bac_ExtraNew
    {
        float fbin_U;
    };

    struct Bac
    {
       Bac_Old      Bac1[AIN21_NUMCH];
       Bac_ExtraNew Bac2[AIN21_NUMCH];
    };

    Bac Bac_block;

    int BoardType;

    void SetupUI();
#if PROGSIZE != PROGSIZE_EMUL
    void SetLbls();
    void SetPf();
    int ShowScheme();
    int ShowU0(int ChNum);
    int ShowI0(int ChNum);
    int ShowI20(int ChNum);
    int ShowU5(int ChNum);

    int Tune();
    int TuneChannel(Check21::Bda &Bda);
    void ShowErrMsg(int);
    bool CalcNewTuneCoef(int NumCh);
    bool CheckTuneCoefs();
    void GetBdAndFillMTT();
#endif
    void FillBac(int bacnum);
    void FillBackBac(int bacnum);

private slots:
    void SetDefCoefs();
#if PROGSIZE != PROGSIZE_EMUL
    int ReadAnalogMeasurements();
    void TuneOneChannel();
    void Cancel();
#endif
};

#endif // TUNEDIALOG21_H
