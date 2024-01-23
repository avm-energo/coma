#pragma once

#include <QObject>
#include <QtTest>

class InterfacesTestObject : public QObject
{
    Q_OBJECT
public:
    explicit InterfacesTestObject(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void modbusRegistersTest01();
    void modbusRegistersTest02();
    void modbusRegistersTest03();
    void modbusRegistersTest04();
    void modbusRegistersTest05();
    void modbusRegistersTest06();
    void modbusRegistersTest07();
    void modbusSinglePoint();

    void iec104ControlBlockTest01();
    void iec104ControlBlockTest02();
    void iec104ControlBlockTest03();
    void iec104ControlBlockTest04();
    void iec104UnnumberedControlTest01();
    void iec104UnnumberedControlTest02();

    void iec104ApciTest01();
    void iec104ApciTest02();

    void iec104AsduTest01();
    void iec104AsduUnpackerTest01();
};
