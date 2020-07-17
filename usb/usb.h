#ifndef USB_H
#define USB_H

#include <QObject>
#include "eusbhid.h"

class usb
{
    Q_OBJECT
public:
    usb();
};

extern EUsbHid *cn;

#endif // USB_H
