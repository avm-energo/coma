#pragma once

#include <tune/abstracttunedialog.h>

class TuneKIVCheck final : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVCheck(Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();

protected:
    void showEvent(QShowEvent *e) override;
};
