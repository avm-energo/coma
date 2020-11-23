#include "../interfaces/protocom.h"
#include "../interfaces/usbhidportinfo.h"

#include <QCoreApplication>
#include <QDebug>

QDebug operator<<(QDebug debug, const UsbHidSettings &settings)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << QString::number(settings.vendor_id, 16) << ":" << QString::number(settings.product_id, 16) << ":"
                    << settings.serial << ":" << settings.path;
    return debug;
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    auto devices = UsbHidPortInfo::devicesFound();
    for (const auto &device : devices)
        qDebug() << device;
    Protocom usbdevice;
    usbdevice.start(devices.first());
    usbdevice.reqBSI();
    usbdevice.reqBSI();
    usbdevice.reqBSI();
    usbdevice.reqFloats(2420, 14);
    usbdevice.reqFloats(2400, 7);
    usbdevice.reqFloats(4501, 2);
    usbdevice.reqFloats(1000, 16);
    usbdevice.reqFloats(2420, 14);
    usbdevice.reqFloats(1100, 16);
    usbdevice.reqFloats(101, 2);
    qDebug() << "Hello world";
    return a.exec();
}
