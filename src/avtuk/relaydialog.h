#pragma once

#include "../widgets/udialog.h"

constexpr uint16_t startRelayReg = 5201;

class RelayDialog : public UDialog
{
public:
    RelayDialog(int relayCount, QWidget *parent = nullptr);

protected:
    QWidget *RelayWidget(int relayCount, QWidget *parent);
    void paintEvent(QPaintEvent *event) override;

private:
    void changeRelay(int number)
    {
        bool state = !relays.at(number);
        changeRelay(number, state);
    }
    void changeRelay(int number, bool state);
    void resetRelays(int count)
    {
        for (auto i = startRelayReg; i != startRelayReg + count; ++i)
        {
            changeRelay(i, false);
        }
    }
    std::map<uint16_t, bool> relays;

    bool isReset = false;
};
