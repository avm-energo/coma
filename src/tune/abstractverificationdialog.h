#pragma once

#include "abstracttunedialog.h"

class AbstractVerificationDialog : public AbstractTuneDialog
{
protected:
public:
    AbstractVerificationDialog(S2::Configuration &config, int tuneStep, QWidget *parent = nullptr);
};
