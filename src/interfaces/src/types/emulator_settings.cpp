#include <interfaces/types/emulator_settings.h>

EmulatorSettings::EmulatorSettings(QObject *parent) : BaseSettings(parent)
{
    set("typeB", 0x00);
    set("typeM", 0x00);
}
