#ifndef TESTPROTOCOM_H
#define TESTPROTOCOM_H
#include "../../gen/stdfunc.h"

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
    void VerToStr();
signals:
private:
};

#endif // TESTPROTOCOM_H
