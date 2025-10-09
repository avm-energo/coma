#pragma once

#include <interfaces/types/base_settings.h>

class EmulatorSettings final : public BaseSettings
{
    Q_OBJECT
public:
    explicit EmulatorSettings(QObject *parent = nullptr);
};
