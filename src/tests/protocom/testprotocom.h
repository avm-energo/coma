#ifndef TESTPROTOCOM_H
#define TESTPROTOCOM_H

#include <QObject>
#include <QtTest>
#include <interfaces/conn/protocom.h>
#include <interfaces/types/usbhidportinfo.h>

class TestProtocom : public QObject
{
    Q_OBJECT
public:
    explicit TestProtocom(QObject *parent = nullptr);
private slots:
    // вызывается перед первой тестовой функцией
    void initTestCase()
    {
        BaseConnection::InterfacePointer device;
        device = BaseConnection::InterfacePointer(new Protocom());
        BaseConnection::setIface(std::move(device));

        auto usbdevice = BaseConnection::iface();
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
