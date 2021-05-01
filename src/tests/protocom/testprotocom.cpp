#include "testprotocom.h"

#include "../../gen/datamanager.h"

#include <QSignalSpy>
TestProtocom::TestProtocom(QObject *parent) : QObject(parent)
{
}

void TestProtocom::testFindDevices()
{
    auto devices = UsbHidPortInfo::devicesFound();
    QVERIFY(!devices.isEmpty());
}

void TestProtocom::testConnection()
{
    auto devices = UsbHidPortInfo::devicesFound();
    if (devices.isEmpty())
    {
        QFAIL("No hid devices");
    }
    QVERIFY(protocom->start(devices.first()));
}

void TestProtocom::testBSIrequest()
{
    const auto &manager = DataManager::GetInstance();
    QSignalSpy spy(&manager, &DataManager::bitStringReceived);
    protocom->reqBSI();
    spy.wait(1000);
    QVERIFY(spy.count() >= 10);
}

QTEST_GUILESS_MAIN(TestProtocom)
