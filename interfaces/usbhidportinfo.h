#pragma once
#include "settingstypes.h"
#include "usbprivate.h"
namespace HID
{
constexpr quint16 VID = 0xC251;
constexpr quint16 PID = 0x3505;
}
class UsbHidPortInfo
{
public:
    UsbHidPortInfo() = delete;

    static QList<UsbHidSettings> devicesFound(quint16 vid = HID::VID);
};
