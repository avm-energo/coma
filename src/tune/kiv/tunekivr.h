#ifndef TUNEKIVMAIN_H
#define TUNEKIVMAIN_H

#include "../../datablocks/kiv/bac2.h"
#include "../../datablocks/kiv/bda.h"
#include "../../datablocks/kiv/bdain.h"
#include "../abstracttunedialog.h"

#include <QWidget>

class TuneKIVR : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVR(ConfigV *config, int tuneStep, QWidget *parent = nullptr);

private:
    Bac2A284 *m_bac;
    BdaA284 *m_bda;
    double m_pt100;
    int m_curTuneStep;
    BdaIn m_bdain;
    int m_BacWidgetIndex, m_BdaWidgetIndex;

    void setTuneFunctions() override;

    Error::Msg showPreWarning();
    Error::Msg setR80();
    Error::Msg processR80();
    Error::Msg setR120();
    Error::Msg processR120();

    void setR(int r);
    double processR(); // get BDA.Pt100 several times and return back value

private slots:
};

#endif // TUNEKIVMAIN_H
