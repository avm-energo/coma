#pragma once

#include "../abstracttunedialog.h"

class Tune85Check : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune85Check(int tuneStep, Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();
private slots:

protected:
    void showEvent(QShowEvent *e) override;
};
