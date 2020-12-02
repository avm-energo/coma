#include "../gen/datamanager.h"
#include "../gen/helper.h"
#include "../interfaces/protocom.h"
#include "console.h"

#include <QCoreApplication>
void dataReceived(const DataTypes::SignalsStruct &);
void bitStringReceived(const DataTypes::BitStringStruct &);
void singlePointReceived(const DataTypes::SinglePointWithTimeStruct &);
void floatReceived(const DataTypes::FloatStruct &);
void fileReceived(const DataTypes::FileStruct &);
void confParametersReceived(const DataTypes::ConfParametersListStruct &);
void responseReceived(const DataTypes::GeneralResponseStruct &);
void blockReceived(const DataTypes::BlockStruct &);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Console logger;

    auto devices = UsbHidPortInfo::devicesFound();
    for (const auto &device : devices)
        qDebug() << device;
    Protocom usbdevice;
    usbdevice.start(devices.first());
    usbdevice.reqBSI();
    usbdevice.writeCommand(Queries::QUSB_GetMode);
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataTech, 1);
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataTech, 2);
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataTech, 3);
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataTech, 4);
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataTech, 5);
    //    usbdevice.reqBSI();
    //    usbdevice.reqBSI();
    auto data = QByteArray::fromHex(
        "000000000000000000000000b81e853e9a99993e9a99993eb81e85be9a9999be9a9999be0000000000000000");
    quint16 blk = 7;
    DataTypes::BlockStruct block { blk, data };
    QVariant value;
    value.setValue(block);
    usbdevice.reqAlarms(3011, 25);
    // usbdevice.reqFloats(2420, 14);
    //    usbdevice.reqBSI();
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    // usbdevice.reqFloats(2400, 7);
    //    usbdevice.reqBSI();
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    // usbdevice.reqFloats(4501, 2);
    //    usbdevice.reqBSI();
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    // usbdevice.reqFloats(1000, 16);
    //    usbdevice.reqBSI();
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    //    usbdevice.reqFloats(2420, 14);
    //    usbdevice.reqBSI();
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    //    usbdevice.reqFloats(1100, 16);
    //    usbdevice.reqBSI();
    //    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    //    usbdevice.reqFloats(101, 2);
    // usbdevice.writeCommand(Queries::QC_WriteUserValues, value);
    qDebug() << "Hello world";
    return a.exec();
}
