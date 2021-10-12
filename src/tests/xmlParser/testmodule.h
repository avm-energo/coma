#ifndef TESTPROTOCOM_H
#define TESTPROTOCOM_H
#include "../../gen/stdfunc.h"
#include "../../module/module.h"

#include <QObject>
#include <QtTest>
class TestModule : public QObject
{
    Q_OBJECT
public:
    explicit TestModule(QObject *parent = nullptr);
private slots:
    // вызывается перед первой тестовой функцией
    void initTestCase()
    {
        Q_INIT_RESOURCE(settings);
        module.setDirectory(resourceDirectory);
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
    void checkA284();
    void checkA284USB();
    void checkA284Eth();
    void checkA284Modbus();

    void check8084();
    void check8084USB();

    void check8085();
    void check8085USB();

    void check8082();
    void check8082USB();

    void check3533();
    void check3533USB();

signals:
private:
    Module module;
};

#endif // TESTPROTOCOM_H
