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

    S2DataTypes::SWJINFStruct SWJInf;
    // EOscillogram::GBoStruct GBOs;
    S2DataTypes::DataRecSwj SwRec;
    EOscillogram *SWJOscFunc;
    bool Mode;
    TrendViewDialog *dlg;
    TrendViewModel *mdl;

    SWJDialog(EOscillogram *osc, int mode = SWJ_MODE_ONLINE, QWidget *parent = nullptr);
    void Init(S2DataTypes::SWJINFStruct swj);
    void LoadOsc(QByteArray &ba); // для оффлайн режима

    void GetSwjOscData();

public slots:
#if PROGSIZE != PROGSIZE_EMUL
    void SaveSWJ();
    void ShowOsc();
#endif
};

#endif // SWJDIALOG_H
