#include "teststdfunc.h"

#include <array>
#include <gen/error.h>
#include <gen/stdfunc.h>

namespace detail
{

template <typename T> T unpackReg(QByteArray ba)
{
    assert(sizeof(T) == ba.size());
    for (auto i = 0; i < ba.size(); i = i + 2)
        std::swap(ba.data()[i], ba.data()[i + 1]);
    return *reinterpret_cast<T *>(ba.data());
}

template <typename T, std::size_t N = sizeof(T)> //
QByteArray packRegister(T value)
{
    static_assert(N % 2 == 0, "The size of type T must be even");
    static_assert(N >= 2, "The size of type T must be greater than or equal to 2");
    QByteArray ba;
    auto srcBegin = reinterpret_cast<std::uint8_t *>(&value);
    auto srcEnd = srcBegin + sizeof(T);
    for (auto it = srcBegin; it != srcEnd; it = it + 2)
    {
        ba.push_back(*(it + 1));
        ba.push_back(*it);
    }
    return ba;
}

} // namespace detail

TestStdFunc::TestStdFunc(QObject *parent) : QObject(parent)
{
}

void TestStdFunc::verToStr()
{
    QString expectedString("3.3-0001");
    auto realString = StdFunc::VerToStr(50528257);
    QVERIFY(expectedString == realString);
}

void TestStdFunc::strToVer()
{
    quint32 expectedValue = 50528257;
    auto realValue = StdFunc::StrToVer("3.3-0001");
    QVERIFY(expectedValue == realValue);
    QVERIFY(StdFunc::StrToVer("3.3-0001") > StdFunc::StrToVer("3.3-0000"));
    QVERIFY(StdFunc::StrToVer("4.3-0001") > StdFunc::StrToVer("3.3-0000"));
}

void TestStdFunc::enumToStr()
{
    auto err = Error::Msg::CrcError;
    auto metaEnum = QMetaEnum::fromType<decltype(err)>();
    QString errStr(metaEnum.valueToKey(err));
    QString expected = "CrcError";
    QVERIFY(errStr == expected);
}

void TestStdFunc::byteArrayTest()
{
    quint16 data = 32125;
    QByteArray first;
    first.append(static_cast<char>(data % 0x100));
    first.append(static_cast<char>(data / 0x100));
    auto second = StdFunc::toByteArray(data);
    QVERIFY(first == second);
}

void TestStdFunc::modbusRegistersTest01()
{
    std::uint16_t data = 0xaabb;
    auto first = StdFunc::toByteArray(qToBigEndian(data));
    auto second = detail::packRegister(data);
    qInfo() << "StdFunc::toByteArray result: " << first;
    qInfo() << "     detail::packReg result: " << second;
    QVERIFY(first == second);
}

void TestStdFunc::modbusRegistersTest02()
{
    std::uint32_t data = 0xaabbccdd;
    auto first = StdFunc::toByteArray(qToBigEndian(data));
    auto second = detail::packRegister(data);
    qInfo() << "StdFunc::toByteArray result: " << first;
    qInfo() << "     detail::packReg result: " << second;
    QVERIFY(first != second);
}

QTEST_GUILESS_MAIN(TestStdFunc)
