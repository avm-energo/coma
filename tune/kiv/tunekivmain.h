#ifndef TUNEKIVMAIN_H
#define TUNEKIVMAIN_H

#include "../eabstracttunedialog.h"
#include "tunekiv.h"
#include <QWidget>

class TuneKIVMain : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVMain(TuneKIV *kiv, QWidget *parent = nullptr);

private:
    TuneKIV *TKIV;
    void SetupUI();
    void SetLbls();
    void SetPf();
    void FillBac(int bacnum);
    void FillBackBac(int bacnum);
    void GetBdAndFill();

private slots:
    int ReadAnalogMeasurements();
    void SetDefCoefs();
};

#endif // TUNEKIVMAIN_H
