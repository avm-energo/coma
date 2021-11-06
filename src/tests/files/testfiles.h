#ifndef TESTPROTOCOM_H
#define TESTPROTOCOM_H

#include <QObject>
#include <QtTest>
class TestFiles : public QObject
{
    Q_OBJECT
public:
    explicit TestFiles(QObject *parent = nullptr);
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
    void testOsc();
    void testOsc85();

    void testSwj();
signals:
private:
};

#endif // TESTPROTOCOM_H
