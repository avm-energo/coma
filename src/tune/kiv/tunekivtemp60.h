#ifndef TUNEKIVTEMP60_H
#define TUNEKIVTEMP60_H

#include "../../datablocks/bd0.h"
#include "../../datablocks/kiv/bac.h"
#include "../../datablocks/kiv/bdain.h"
#include "../abstracttunedialog.h"

#include <QWidget>

class TuneKIVTemp60 : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVTemp60(ConfigV *config, int tuneStep, QWidget *parent = nullptr);

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

    Bac *m_bac;
    BdaIn *m_bdain;
    Bd0 *m_bd0;
    MidTuneStruct m_midTuneStruct;
    //    const QVector<TuneDescrStruct> m_tuneDescrVector()
    //    {
    //        QVector<TuneDescrStruct> vect;
    //        for (int i = 0; i < 6; ++i)
    //            vect.append({ "u_p" + QString::number(i), &m_midTuneStruct.u[i] });
    //        for (int i = 0; i < 3; ++i)
    //            vect.append({ "y_p" + QString::number(i), &m_midTuneStruct.y[i] });
    //        vect.append({ "tmk_p", &m_midTuneStruct.tmk });
    //        vect.append({ "uet_p", &m_midTuneStruct.uet });
    //        vect.append({ "iet_p", &m_midTuneStruct.iet });
    //        vect.append({ "yet_p", &m_midTuneStruct.yet });
    //        return vect;
    //    };

    void setMessages();
    void setTuneFunctions();
    //    void FillBac(int bacnum);
    //    void FillBackBac(int bacnum);
    //    QWidget *MainUI();
    //    void GetBdAndFill();
    //    Error::Msg LoadTuneSequenceFile();

    Error::Msg setNewConfAndTune();
    Error::Msg showTempDialog();
    Error::Msg waitForTempToRise();
    Error::Msg showSignalsDialog();
    Error::Msg analogMeasurement();
    Error::Msg inputEnergomonitorValues();

    Error::Msg calcTuneCoefs();
    void loadIntermediateResults();

signals:
    void closeShowTuneDialog();

private slots:
    void saveIntermediateResults();
    //    void acceptTuneCoefs();
    //    int ReadAnalogMeasurements();
    //    void setDefCoefs();
};

#endif // TUNEKIVTEMP60_H
