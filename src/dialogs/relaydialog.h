#pragma once

#include "../widgets/udialog.h"

constexpr uint16_t startRelayReg = 5201;

class RelayDialog : public UDialog
{
public:
    RelayDialog(int relayCount, Device::CurrentDevice *device, QWidget *parent = nullptr);

protected:
    QWidget *RelayWidget(int relayCount, QWidget *parent);
    void paintEvent(QPaintEvent *event) override;

private:
    void changeRelay(int number);
    void changeRelay(int number, bool state);
    void resetRelays(int count);

    std::map<uint16_t, bool> relays;
    bool isReset = false;
};
