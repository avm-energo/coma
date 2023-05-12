#pragma once

#include "baseinterface.h"

struct UsbHidSettings;

using namespace Interface;

class Protocom final : public BaseInterface
{
    Q_OBJECT

public:
    explicit Protocom(QObject *parent = nullptr);
    ~Protocom();
    bool start(const ConnectStruct &st) override;
    bool start(const UsbHidSettings &usbhid);
};
