#pragma once

#include "../abstracttunedialog.h"

class Tune84Check : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune84Check(int tuneStep, Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();
private slots:

protected:
    void showEvent(QShowEvent *e) override;
};
