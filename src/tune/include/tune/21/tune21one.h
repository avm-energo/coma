#pragma once

#include <datablocks/21/bac.h>
#include <datablocks/21/bda.h>
#include <tune/abstracttunedialog.h>

class Tune21One final : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune21One(Device::BoardTypes boardType, u8 chNum, Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    Bac21 *m_bac;
    Bda21 *m_bda;
    u8 m_channelNumber;

    void setTuneFunctions() override;

    Error::Msg sendBac();
    Error::Msg showScheme();
    Error::Msg checkTuneCoefs();
    Error::Msg tune();
    bool calcNewTuneCoefs(float u0, float u5, float i0, float i20);
};
