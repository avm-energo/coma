#ifndef TUNEKIVMAIN_H
#define TUNEKIVMAIN_H

#include "../eabstracttunedialog.h"
#include "tunekiv.h"
#include <QWidget>

class TuneKIVCheck : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVCheck(TuneKIV *kiv, QWidget *parent = nullptr);

private:
    TuneKIV *TKIV;
    void SetupUI();
    void SetLbls();
    void SetPf();
    void FillBac(int bacnum);
    void FillBackBac(int bacnum);
    void GetBdAndFill();
    Error::Msg SaveWorkConfig();

private slots:
    int ReadAnalogMeasurements();
    void SetDefCoefs();
};

#endif // TUNEKIVMAIN_H
