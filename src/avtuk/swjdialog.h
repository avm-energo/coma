#ifndef SWJDIALOG_H
#define SWJDIALOG_H

#include "../gen/datatypes.h"
#include "eoscillogram.h"
#include "oscmanager.h"
#include "swjmanager.h"

#include <QDialog>

class TrendViewDialog;

class SWJDialog : public QDialog
{
    Q_OBJECT
public:
    enum Modes
    {
        SWJ_MODE_OFFLINE,
        SWJ_MODE_ONLINE
    };

    SWJDialog(std::unique_ptr<EOscillogram> osc, int mode = SWJ_MODE_ONLINE, QWidget *parent = nullptr);
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
    OscManager oscManager;
    SwjManager swjManager;

    S2DataTypes::DataRecSwitchJour SwRec;
    std::unique_ptr<EOscillogram> SWJOscFunc;
    bool Mode;
    TrendViewDialog *m_trendViewDialog;
};

#endif // SWJDIALOG_H
