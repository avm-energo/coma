#ifndef TUNEKIVTEMP60_H
#define TUNEKIVTEMP60_H

#include "../../config/configkiv.h"
#include "../abstracttunedialog.h"
#include "tunekiv.h"

#include <QWidget>

class TuneKIVTemp60 : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVTemp60(int tuneStep, ConfigKIV *ckiv, TuneKIV *tkiv, QWidget *parent = nullptr);

private:
    struct MidTuneStruct
    {
        float u[6];
        float y[3];
        float tmk;
        float uet;
        float iet;
        float yet;
    };

    TuneKIV *TKIV;
    ConfigKIV *CKIV;
    MidTuneStruct m_midTuneStruct;

    void setMessages();
    void setTuneFunctions();
    void FillBac(int bacnum);
    void FillBackBac(int bacnum);
    QWidget *MainUI();
    //    void GetBdAndFill();
    //    Error::Msg LoadTuneSequenceFile();

    Error::Msg setNewConfAndTune();
    Error::Msg showTempDialog();
    Error::Msg waitForTempToRise();
    Error::Msg showSignalsDialog();
    Error::Msg analogMeasurement();
    Error::Msg inputEnergomonitorValues();
    Error::Msg calcTuneCoefsAndWrite();
    void loadIntermediateResults(MidTuneStruct &tunestr);

private slots:
    void saveIntermediateResults();
    //    int ReadAnalogMeasurements();
    //    void SetDefCoefs();
};

#endif // TUNEKIVTEMP60_H
