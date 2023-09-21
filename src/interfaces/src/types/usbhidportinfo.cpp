#include "interfaces/types/usbhidportinfo.h"

#include <hidapi/hidapi.h>

QList<UsbHidSettings> UsbHidPortInfo::devicesFound(quint16 vid)
{
    hid_device_info *devs, *cur_dev;
    // Enumerate all, extract only needed
    devs = hid_enumerate(vid, 0x0);
    cur_dev = devs;
    QList<UsbHidSettings> sl;
    while (cur_dev)
    {
        UsbHidSettings buffer {
            cur_dev->vendor_id,                              // Vendor ID
            cur_dev->product_id,                             // Product ID
            QString::fromWCharArray(cur_dev->serial_number), // Serial number
#ifdef QT_DEBUG
            QString(cur_dev->path) // Path
#endif
        };

        sl.push_back(buffer);

        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    return sl;
}
