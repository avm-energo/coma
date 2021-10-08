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

void TestStdFunc::StrToVer()
{
    quint32 expectedValue = 50528257;
    auto realValue = StdFunc::StrToVer("3.3-0001");
    QVERIFY(expectedValue == realValue);
}

QTEST_GUILESS_MAIN(TestStdFunc)
