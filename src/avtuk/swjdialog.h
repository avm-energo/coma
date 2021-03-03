#ifndef SWJDIALOG_H
#define SWJDIALOG_H

#include "eoscillogram.h"
#include "trendviewdialog.h"

#include <QDialog>

class SWJDialog : public QDialog
{
    Q_OBJECT
public:
    enum Modes
    {
        SWJ_MODE_OFFLINE,
        SWJ_MODE_ONLINE
    };

    SWJDialog(EOscillogram *osc, int mode = SWJ_MODE_ONLINE, QWidget *parent = nullptr);
    void Init(S2DataTypes::SwitchJourInfo swj);
    void LoadOsc(QByteArray &ba); // для оффлайн режима

    void GetSwjOscData();

    TrendViewDialog *trendViewDialog() const;

public slots:
#if PROGSIZE != PROGSIZE_EMUL
    void SaveSWJ();
    void ShowOsc();
#endif
private:
    S2DataTypes::SwitchJourInfo SWJInf;
    // EOscillogram::GBoStruct GBOs;
    S2DataTypes::DataRecSwitchJour SwRec;
    EOscillogram *SWJOscFunc;
    bool Mode;
    TrendViewDialog *m_trendViewDialog;
    // TrendViewModel *mdl;
};

#endif // SWJDIALOG_H
