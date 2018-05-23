#ifndef TUNEDIALOG21_H
#define TUNEDIALOG21_H

#include <QDialog>
#include "eabstracttunedialog.h"
#include "../check/check21.h"

#define TUNEFILELENGTH  256

#define ATUNEER(a)       ERMSG(publicclass::ER_ATUNE,__LINE__,a)
#define ATUNEDBG         DBGMSG(publicclass::ER_ATUNE,__LINE__)
#define ATUNEWARN(a)    WARNMSG(publicclass::ER_ATUNE,__LINE__,a)
#define ATUNEINFO(a)     INFOMSG(publicclass::ER_ATUNE,__LINE__,a)

#define ATUNE_U0    0
#define ATUNE_I20   1
#define ATUNE_U5    2

class TuneDialog21 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialog21(int type = 0x01, QWidget *parent = 0);
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
    int ChNum;

    void SetupUI();
    void SetLbls();
    void SetPf();
    void FillBac();
    void FillBackBac();
    void GetBdAndFillMTT();
    int ShowScheme();
    int ShowU0();
    int ShowI20();
    int ShowU5();

    int Tune();
    int TuneChannel(int Type);
    void ShowErrMsg(int);
    bool CalcNewTuneCoef();
    bool CheckAndShowTune0();
    bool CheckAndShowTune5();
    bool CheckAndShowTune20();
    bool CheckTuneCoefs();

private slots:
    void SetDefCoefs();
    int ReadAnalogMeasurements();
    void TuneOneChannel();
};

#endif // TUNEDIALOG21_H
