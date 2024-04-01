#pragma once

#include "../generaltunedialog.h"

class Tune82Dialog : public GeneralTuneDialog
{
    Q_OBJECT
public:
    explicit Tune82Dialog(Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    void prepareReport() override;
};
