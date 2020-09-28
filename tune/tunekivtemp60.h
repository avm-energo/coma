#ifndef TUNEKIVTEMP60_H
#define TUNEKIVTEMP60_H

#include "eabstracttunedialog.h"
#include <QWidget>

class TuneKIVTemp60 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVTemp60(QWidget *parent = nullptr);

private:
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

#endif // TUNEKIVTEMP60_H
