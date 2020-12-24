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
    //    usbdevice.reqBSI();
    usbdevice.writeCommand(Queries::QUSB_GetMode);
    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataA, 1);
       usbdevice.writeCommand(Queries::QUSB_ReqBlkData, 1);
    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataTech, 1);
    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataTech, 2);
    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataTech, 3);
    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataTech, 4);
    usbdevice.writeCommand(Queries::QUSB_ReqBlkDataTech, 5);
    //    usbdevice.reqBSI();
    //    usbdevice.reqBSI();
    auto data = QByteArray::fromHex(
        "000000000000000000000000b81e853e9a99993e9a99993eb81e85be9a9999be9a9999be0000000000000000");
    quint16 blk = 7;
    DataTypes::BlockStruct block { blk, data };
    QVariant value;
    value.setValue(block);
    //    usbdevice.reqAlarms(3011, 25);
    // usbdevice.reqFile(DataTypes::FilesEnum::JourSys);
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
    usbdevice.writeCommand(Queries::QC_WriteUserValues, value);
    /// 3e323c000100ffffc6020000b8dc427a21b7c85feb0300000400000064000000ee0300000400000000005c43f4030000040000000000003ff503000004000000
    /// 3e323b000100ffffc6020000a46958f93645ca5f01000000040000000000000002000000040000000000000003000000040000000000000004000000040000
    /// 3e323c00000100ffffc6020000a46958f90a44ca5f01000000040000000000000002000000040000000000000003000000040000000000000004000000040000
    /// 3e323c00010100ffffc6020000a46958f9a01eca5f01000000040000000000000002000000040000000000000003000000040000000000000004000000040000
    //    auto ba =
    //    QByteArray::fromHex("3e323c000100ffffc6020000b8dc427a21b7c85feb0300000400000064000000ee030000040000000000"
    //                                  "5c43f4030000040000000000003ff503000004000000");
    //    usbdevice.writeRaw(ba);
    qDebug() << "Hello world";
    return a.exec();
}
