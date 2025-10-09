#pragma once

#include <interfaces/types/emulator_settings.h>
#include <interfaces/types/iec104_settings.h>
#include <interfaces/types/serial_settings.h>
#include <interfaces/types/usbhid_settings.h>

struct ConnectionSettings
{
    QString name;
    std::variant<IEC104Settings *, SerialSettings *, UsbHidSettings *, EmulatorSettings *> settings;
};
