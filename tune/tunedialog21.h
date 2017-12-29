#ifndef TUNEDIALOG21_H
#define TUNEDIALOG21_H

#include <QDialog>
#include "eabstracttunedialog.h"
#include "../check/check21.h"

#define ATUNENUMPOINTS  64 // количество точек, по которым выдаются значения в блоке Bda

#define TUNEFILELENGTH  256

#define ATUNEER(a)       ERMSG(publicclass::ER_ATUNE,__LINE__,a)
#define ATUNEDBG         DBGMSG(publicclass::ER_ATUNE,__LINE__)
#define ATUNEWARN(a)    WARNMSG(publicclass::ER_ATUNE,__LINE__,a)
#define ATUNEINFO(a)     INFOMSG(publicclass::ER_ATUNE,__LINE__,a)

class TuneDialog21 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialog21(int type = BT_BASE, QWidget *parent = 0);

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
    void SetLbls();
    void SetPf();
    void FillBac();
    void FillBackBac();
    void GetBdAndFillMTT();
    int ShowScheme();
    void StartTune();

    int Tune();
    bool TuneChannel(int Type, int ChNum);
    void ShowErrMsg(int);
    bool CalcNewTuneCoef(int ChNum);
    bool CheckAndShowTune0(int ChNum);
    bool CheckAndShowTune5(int ChNum);
    bool CheckAndShowTune20(int ChNum);
    bool CheckTuneCoefs();

private slots:
    void SetDefCoefs();
    int ReadAnalogMeasurements();
};

#endif // TUNEDIALOG21_H
