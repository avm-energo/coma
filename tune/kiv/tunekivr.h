#ifndef TUNEKIVMAIN_H
#define TUNEKIVMAIN_H

#include "../../config/configkiv.h"
#include "../abstracttunedialog.h"
#include "tunekiv.h"

#include <QWidget>

class TuneKIVR : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVR(int tuneStep, ConfigKIV *ckiv, TuneKIV *kiv, QWidget *parent = nullptr);

private:
    TuneKIV *TKIV;
    ConfigKIV *CKIV;
    double m_pt100;
    int m_curTuneStep;
    TuneKIV::Bda_in m_bdain;

    void setMessages() override;
    void setTuneFunctions() override;
    QWidget *MainUI() override;

    Error::Msg showPreWarning();
    Error::Msg checkTuneCoefs();
    Error::Msg setR80();
    Error::Msg processR80();
    Error::Msg setR120();
    Error::Msg processR120();

    void setR(int r);
    double processR(); // get BDA.Pt100 several times and return back value

    void showTWTab(int num);

private slots:
    void setDefCoefs() override;
};

#endif // TUNEKIVMAIN_H
