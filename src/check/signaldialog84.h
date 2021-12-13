#pragma once

#include "../datablocks/kiv/bdain.h"
#include "../gen/stdfunc.h"
#include "signaldialog.h"

class SignalDialog84 final : public SignalDialog
{
    Q_OBJECT
public:
    explicit SignalDialog84(QWidget *parent = nullptr);
    void reqUpdate() override;

private:
    UniquePointer<BdaIn> m_bdaIn;
};
