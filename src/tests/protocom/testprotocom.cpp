#include "testprotocom.h"

#include <QSignalSpy>
#include <gen/stdfunc.h>

TestProtocom::TestProtocom(QObject *parent) : QObject(parent)
{
}

void TestProtocom::testFindDevices()
{
    //    auto devices = UsbHidPortInfo::devicesFound();
    //    QVERIFY(!devices.isEmpty());
}

void TestProtocom::testConnection()
{
    //    auto devices = UsbHidPortInfo::devicesFound();
    //    if (devices.isEmpty())
    //    {
    //        QFAIL("No hid devices");
    //    }
    //    QVERIFY(protocom->start(devices.first()));
}

void TestProtocom::testBSIrequest()
{
    //    const auto &manager = DataManager::GetInstance();
    //    QSignalSpy spy(&manager, &DataManager::DataReceived);
    //    protocom->reqBSI();
    //    StdFunc::Wait(1000);
    //    // spy.wait();
    //    qDebug() << spy.count();
    //    QCOMPARE(spy.count(), 15);
}

void TestProtocom::testBSIErequest()
{
    //    const auto &manager = DataManager::GetInstance();
    //    QSignalSpy spy(&manager, &DataManager::DataReceived);
    //    protocom->reqBSIExt();
    //    StdFunc::Wait(1000);
    //    qDebug() << spy.count();
    //    QVERIFY(spy.count() >= 5);
}

QTEST_GUILESS_MAIN(TestProtocom)
