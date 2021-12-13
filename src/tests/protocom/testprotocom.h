#ifndef TESTPROTOCOM_H
#define TESTPROTOCOM_H
#include "../../interfaces/protocom.h"
#include "../../interfaces/usbhidportinfo.h"

#include <QObject>
#include <QtTest>
class TestProtocom : public QObject
{
    Q_OBJECT
public:
    explicit TestProtocom(QObject *parent = nullptr);
private slots:
    // вызывается перед первой тестовой функцией
    void initTestCase()
    {
        BaseInterface::InterfacePointer device;
        device = BaseInterface::InterfacePointer(new Protocom());
        BaseInterface::setIface(std::move(device));

        auto usbdevice = BaseInterface::iface();
        protocom = static_cast<Protocom *>(usbdevice);
    }
    // вызывается перед каждой тестовой функцией
    void init()
    {
    }
    // вызывается после каждой тестовой функции
    void cleanup()
    {
    }
    // вызывается после последней тестовой функции
    void cleanupTestCase()
    {
    }

    void testFindDevices();
    void testConnection();
    void testBSIrequest();
    void testBSIErequest();
signals:
private:
    Protocom *protocom;
};

#endif // TESTPROTOCOM_H
