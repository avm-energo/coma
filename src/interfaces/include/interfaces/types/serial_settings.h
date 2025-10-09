#pragma once

#include <interfaces/types/iec104_settings.h>
#include <interfaces/types/usbhid_settings.h>

class SerialSettings final : public BaseSettings
{
    Q_OBJECT
public:
    explicit SerialSettings(QObject *parent = nullptr);

    bool isValid();
};
