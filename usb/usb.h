#ifndef USB_H
#define USB_H

#include <QObject>

#include "eusbhid.h"

class usb : public QObject
{
    Q_OBJECT
public:
    usb(QObject *parent = nullptr);
};

extern EUsbHid *cn;

#endif // USB_H
