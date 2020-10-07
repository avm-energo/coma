#ifndef TUNEKIVTEMP_20_H
#define TUNEKIVTEMP_20_H

#include "../abstracttunedialog.h"
#include "tunekiv.h"

#include <QWidget>

class TuneKIVTemp_20 : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVTemp_20(TuneKIV *tkiv, QWidget *parent = nullptr);

private:
    TuneKIV *TKIV;
    void SetupUI();
    void SetLbls();
    void SetPf();
    void FillBac(int bacnum);
    void FillBackBac(int bacnum);
    void GetBdAndFill();
    Error::Msg LoadTuneSequenceFile();

private slots:
    int ReadAnalogMeasurements();
    void SetDefCoefs();
};

#endif // TUNEKIVTEMP_20_H
