#include "../gen/datamanager.h"
#include "../gen/stdfunc.h"
#include "../interfaces/baseinterface.h"
#include "../interfaces/protocom.h"
#include "../interfaces/usbhidportinfo.h"

#include <QCoreApplication>
#include <QObject>
#include <iostream>
void print(const DataTypes::BitStringStruct &st)
{
    std::cout << "BitString {"
              << "Addr:" << st.sigAdr << ","
              << "Val:" << st.sigVal << ","
              << "Qual:" << st.sigQuality << " }" << std::endl;
}

int main(int argc, char *argv[])
{
    std::cout << "Started " << std::endl;

    QCoreApplication a(argc, argv);
    const auto devices = UsbHidPortInfo::devicesFound(0x0483);
    if (devices.isEmpty())
    {
        std::cout<<"No devices"<<std::endl;
        return a.exec();
    }
    for (const auto &device : devices)
    {
        std::cout << "Vendor id:" << std::hex << device.vendor_id << std::dec << " : "
                  << "Product id:" << std::hex << device.product_id << std::dec << " : "
                  << "Serial number:" << device.serial.toStdString() << std::endl;
    }
    BaseInterface::InterfacePointer device;
    device = BaseInterface::InterfacePointer(new Protocom());
    BaseInterface::setIface(std::move(device));

    auto protocom = static_cast<Protocom *>(BaseInterface::iface());
    if (!protocom->start(devices.first()))
        std::cout<<"Couldnt connect"<<std::endl;
    const auto &manager = DataManager::GetInstance();
    QObject::connect(&manager, &DataManager::bitStringReceived, &print);
    protocom->reqBSI();
    StdFunc::Wait(1000);
    std::cout << "Finished" << std::endl;
    return a.exec();
}
