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

    void check2100();
    void check2121();
    void check2122();
    void check2131();
    void check2133();
    void check2134();
    void check2135();

    void check2200();
    void check2221();
    void check2222();
    void check2231();
    void check2233();
    void check2234();
    void check2235();

    void check3100();
    void check3121();
    void check3122();
    void check3131();
    void check3133();
    void check3134();
    void check3135();

    void check3300();
    void check3321();
    void check3322();
    void check3331();
    void check3333();
    void check3334();
    void check3335();

    void check3400();
    void check3421();
    void check3422();
    void check3431();
    void check3433();
    void check3434();
    void check3435();

    void check3500();
    void check3521();
    void check3522();
    void check3531();
    void check3533();
    void check3534();
    void check3535();

    void check3533USB();

    void check8081();
    void check8081USB();

    void check8082();
    void check8082USB();

    void check8083();
    void check8083USB();

    void check8084();
    void check8084USB();

    void check8585();
    void check8585USB();

    void check8600();
    void check8600USB();

    void checkA284();
    void checkA284USB();
    void checkA284Eth();
    void checkA284Modbus();

    void checkA287();
    void checkA287USB();
    void checkA287Eth();

    void checkA387();
    void checkA387USB();
    void checkA387Eth();

signals:
private:
    Module module;
};

#endif // TESTPROTOCOM_H
