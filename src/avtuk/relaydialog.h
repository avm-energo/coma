#pragma once

#include "../widgets/udialog.h"

constexpr uint16_t startRelayReg = 5201;

class RelayDialog : public UDialog
{
public:
    RelayDialog(int relayCount, QWidget *parent = nullptr);

protected:
    QWidget *RelayWidget(int relayCount, QWidget *parent);

private:
    void changeRelay(int number);
    std::map<uint16_t, bool> relays;
};
