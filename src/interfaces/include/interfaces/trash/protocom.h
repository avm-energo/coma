#pragma once

#include <interfaces/conn/baseconnection.h>

struct UsbHidSettings;

using namespace Interface;

class Protocom final : public BaseConnection
{
    Q_OBJECT

public:
    explicit Protocom(QObject *parent = nullptr);
    ~Protocom();
    bool start(const ConnectStruct &st) override;
    bool start(const UsbHidSettings &usbhid);
};
