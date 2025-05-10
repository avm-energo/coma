#pragma once

#include <datablocks/21/bac.h>
#include <datablocks/21/bda.h>
#include <tune/abstracttunedialog.h>

class Tune21One final : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune21One(Device::BoardTypes boardType, Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    Bac21 *m_bac;
    Bda21 *m_bda;
    float m_i0[8], m_i20[8], m_u0[8], m_u5[8];

    void setTuneFunctions() override;

    Error::Msg sendBac();
    Error::Msg showScheme();
    Error::Msg checkTuneCoefs();
    Error::Msg tune0();
    Error::Msg tune4();
    Error::Msg tune5();
    Error::Msg tune20();
    Error::Msg calcNewTuneCoefs();
};
