#include <interfaces/types/serial_settings.h>

#include <QSerialPort>

SerialSettings::SerialSettings(QObject *parent) : BaseSettings(parent)
{
    set("name", "/dev/ttyUSB0");
    set("baud", 115200);
    set("address", 0x01);
    set("parity", QSerialPort::Parity::NoParity);
    set("stop", QSerialPort::StopBits::OneStop);
}

bool SerialSettings::isValid()
{
    return !(QString(get("name")).isEmpty());
}
