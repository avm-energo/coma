#ifndef TUNEKIVTEMP60_H
#define TUNEKIVTEMP60_H

#include "../abstracttunedialog.h"
#include <QWidget>

class TuneKIVTemp60 : public AbstractTuneDialog
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
