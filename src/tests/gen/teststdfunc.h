#ifndef TESTPROTOCOM_H
#define TESTPROTOCOM_H

#include <QObject>
#include <QtTest>

class TestStdFunc : public QObject
{
    Q_OBJECT
public:
    explicit TestStdFunc(QObject *parent = nullptr);

private slots:
    // вызывается перед первой тестовой функцией
    void initTestCase()
    {
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

    void verToStr();
    void strToVer();
    void enumToStr();
    void byteArrayTest01();
    void byteArrayTest02();

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
};

#endif // TESTPROTOCOM_H
