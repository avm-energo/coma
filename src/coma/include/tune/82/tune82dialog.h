#pragma once

#include <tune/generaltunedialog.h>

class Tune82Dialog final : public GeneralTuneDialog
{
    Q_OBJECT
public:
    explicit Tune82Dialog(Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    void prepareReport() override;
};
