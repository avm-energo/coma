#include "interfaces_test_obj.h"

InterfacesTestObject::InterfacesTestObject(QObject *parent) : QObject(parent)
{
}

// вызывается перед первой тестовой функцией
void InterfacesTestObject::initTestCase()
{
}

// вызывается перед каждой тестовой функцией
void InterfacesTestObject::init()
{
}

// вызывается после каждой тестовой функции
void InterfacesTestObject::cleanup()
{
}

// вызывается после последней тестовой функции
void InterfacesTestObject::cleanupTestCase()
{
}

QTEST_GUILESS_MAIN(InterfacesTestObject)
