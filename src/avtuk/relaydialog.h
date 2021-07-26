#pragma once

#include "../widgets/udialog.h"

class RelayDialog : public UDialog
{
public:
    RelayDialog(int relayCount, QWidget *parent = nullptr);

protected:
    QWidget *RelayWidget(int relayCount, QWidget *parent);
};
