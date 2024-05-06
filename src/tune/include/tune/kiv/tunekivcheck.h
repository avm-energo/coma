#pragma once

#include <tune/abstracttunedialog.h>

class TuneKIVCheck final : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVCheck(int tuneStep, Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();
    Error::Msg setSMode2();

protected:
    void showEvent(QShowEvent *e) override;
};
