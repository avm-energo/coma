#ifndef TUNEDIALOG21_H
#define TUNEDIALOG21_H

#include <QDialog>
#include "eabstracttunedialog.h"
#include "../check/check21.h"
#include "../gen/modulebsi.h"

#define TUNEFILELENGTH  256

#define ATUNE_U0    0
#define ATUNE_I20   1
#define ATUNE_U5    2

class TuneDialog21 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialog21(BoardTypes type = BoardTypes::BT_BASE, QWidget *parent = 0);
    ~TuneDialog21();

signals:

public slots:

private:
    Check21::Bda Bda0, Bda5, Bda20;

    struct Bac
    {
        float fbin;
        float fkuin;
        float fkiin;
    };

    Bac Bac_block[AIN21_NUMCH];

    int BoardType;

    void SetupUI();
#if PROGSIZE != PROGSIZE_EMUL
    void SetLbls();
    void SetPf();
    int ShowScheme();
    int ShowU0(int ChNum);
    int ShowI20(int ChNum);
    int ShowU5(int ChNum);

    int Tune();
    int TuneChannel(Check21::Bda &Bda);
    void ShowErrMsg(int);
    bool CalcNewTuneCoef(int NumCh);
    bool CheckTuneCoefs();
#endif
    void FillBac();
    void FillBackBac();
    void GetBdAndFillMTT();

private slots:
    void SetDefCoefs();
#if PROGSIZE != PROGSIZE_EMUL
    int ReadAnalogMeasurements();
    void TuneOneChannel();
#endif
};

#endif // TUNEDIALOG21_H
