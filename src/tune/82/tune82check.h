#pragma once

#include "../abstracttunedialog.h"

class Tune82Check : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune82Check(int tuneStep, Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();
    Error::Msg checkMip();

protected:
    void showEvent(QShowEvent *e) override;
};
