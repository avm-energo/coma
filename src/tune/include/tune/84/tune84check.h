#pragma once

#include <tune/abstracttunedialog.h>

class Tune84Check final : public AbstractTuneDialog
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
