#pragma once

#include <QMap>

namespace Constants
{
enum PathsEnum
{
    FROSYA,
    STARTICON,
    STOPICON,
    SETTINGSICON,
    USBICON,
    RS485ICON,
    ETHERNETICON
};

const QMap<PathsEnum, QString> Resources {      //
    { FROSYA, ":/icons/tnfrosya.svg" },         //
    { STARTICON, ":/icons/tnstart.svg" },       //
    { STOPICON, ":/icons/tnstop.svg" },         //
    { SETTINGSICON, ":/icons/tnsettings.svg" }, //
    { USBICON, ":/icons/usb.svg" },             //
    { RS485ICON, ":/icons/rs485.svg" },         //
    { ETHERNETICON, ":/icons/ethernet.svg" }
};
}
