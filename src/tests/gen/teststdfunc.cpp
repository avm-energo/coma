#include "teststdfunc.h"

TestStdFunc::TestStdFunc(QObject *parent) : QObject(parent)
{
}

void TestStdFunc::VerToStr()
{
    QString expectedString("3.3-0001");
    auto realString = StdFunc::VerToStr(50528257);
    QVERIFY(expectedString == realString);
}

QTEST_GUILESS_MAIN(TestStdFunc)
