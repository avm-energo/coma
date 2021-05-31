#include "../gen/datamanager.h"
#include "../gen/stdfunc.h"
#include "../interfaces/baseinterface.h"
#include "../interfaces/protocom.h"
#include "../interfaces/usbhidportinfo.h"
#include "grpc_sync_server.h"
#include "helper.h"
#include "timesyncronizer.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QObject>
//#include <grpc/grpc.h>
//#include "../grpc_proto/grpc_proto.grpc.pb.h"
//#include "grpc_proto.pb.h"
//#include "grpc_async_server.h"
void printbs(const DataTypes::BitStringStruct &st)
{
    std::cout << "BitString {"
              << "Addr:" << st.sigAdr << ","
              << "Val:" << st.sigVal << ","
              << "Qual:" << st.sigQuality << " }" << std::endl;
}

void alise_test()
{
    const auto devices = UsbHidPortInfo::devicesFound(0x0483);
    if (devices.isEmpty())
    {
        std::cout << "No devices" << std::endl;
        return;
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
        std::cout << "Couldnt connect" << std::endl;
    const auto &manager = DataManager::GetInstance();
    QObject::connect(&manager, &DataManager::bitStringReceived, &printbs);
    TimeSyncronizer timeSync;
    if (timeSync.isNtpSync())
        std::cout << "Ntp enabled";
    QObject::connect(&manager, &DataManager::timeReceived, &timeSync, &TimeSyncronizer::handleTime);
    QObject::connect(&timeSync, &TimeSyncronizer::sendTime, protocom, [&](const timespec &time) {
        protocom->writeTime(time);
        protocom->reqTime();
    });
    protocom->reqBSI();
    protocom->reqTime();
    StdFunc::Wait(1000);
    std::cout << "Finished" << std::endl;
    QTimer::singleShot(0, []() {
        // do stuff
        QCoreApplication::exit(0);
    });
}

int main(int argc, char *argv[])
{
    std::cout << "Started " << std::endl;

    QCoreApplication a(argc, argv);
    ServerController controller;
    controller.launch();
    // alise_test();
    std::cout << "Finished" << std::endl;
    return a.exec();
}
