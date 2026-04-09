#pragma once

#include <datablocks/kiv/bac.h>
#include <datablocks/kiv/bda.h>
#include <datablocks/kiv/bdain.h>
#include <tune/abstracttunedialog.h>

class TuneKIVR final : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVR(Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    BacA284 *m_bac;
    BdaA284 *m_bda;
    double m_pt100_80;
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
