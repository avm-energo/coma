#include "interfaces_test_obj.h"

#include <gen/datatypes.h>
#include <gen/stdfunc.h>
#include <interfaces/utils/modbus_convertations.h>

/* * * * * * * * * * * * * * * * * * * * * * * *
 * Тесты на совместимость старого кода с новым *
 * * * * * * * * * * * * * * * * * * * * * * * */

namespace detail
{

template <typename T> //
inline T unpackReg(QByteArray ba)
{
    assert(sizeof(T) == ba.size());
    for (auto i = 0; i < ba.size(); i = i + 2)
        std::swap(ba.data()[i], ba.data()[i + 1]);
    return *reinterpret_cast<T *>(ba.data());
}

template <typename T, std::size_t N = sizeof(T)> //
inline QByteArray packReg(T value)
{
    QByteArray ba;
    ba.reserve(N);
    auto srcBegin = reinterpret_cast<std::uint16_t *>(&value);
    auto srcEnd = srcBegin + (N / sizeof(std::uint16_t));
    for (auto iter = srcBegin; iter != srcEnd; ++iter)
        ba.append(StdFunc::toByteArray(qToBigEndian(*iter)));
    return ba;
}

using SinglePoints = std::vector<DataTypes::SinglePointWithTimeStruct>;

SinglePoints processSinglePointSignals01(const QByteArray &data, const quint16 addr) noexcept
{
    SinglePoints result;
    result.reserve(data.size() * 8);
    for (int i = 0; i < data.size(); ++i)
    {
        quint8 ival = data[i];
        DataTypes::SinglePointWithTimeStruct signal;
        for (int j = 0; j < 8; ++j)
        {
            signal.sigAdr = addr + i * 8 + j;
            signal.sigVal = ((0x01 << j) & ival) ? 1 : 0;
            signal.sigQuality = DataTypes::Quality::Good;
            result.push_back(signal);
        }
    }
    return result;
}

SinglePoints processSinglePointSignals02(const QByteArray &data, const quint16 addr) noexcept
{
    SinglePoints result;
    result.reserve(data.size() * 8);
    for (int i = 0; i < data.size(); ++i)
    {
        std::bitset<8> byte(static_cast<quint8>(data[i]));
        DataTypes::SinglePointWithTimeStruct signal;
        for (int j = 0; j < 8; ++j)
        {
            signal.sigAdr = addr + i * 8 + j;
            signal.sigVal = byte.test(j);
            signal.sigQuality = DataTypes::Quality::Good;
            result.push_back(signal);
        }
    }
    return result;
}

bool compare(const SinglePoints &lhs, const SinglePoints &rhs) noexcept
{
    if (lhs.size() == rhs.size())
    {
        bool flag = true;
        for (auto i = 0; i < lhs.size(); ++i)
        {
            const auto &lhsItem = lhs[i], &rhsItem = rhs[i];
            if ((lhsItem.sigAdr != rhsItem.sigAdr)             //
                || (lhsItem.sigVal != rhsItem.sigVal)          //
                || (lhsItem.sigQuality != rhsItem.sigQuality)) //
            {
                flag = false;
                break;
            }
        }
        return flag;
    }
    return false;
}

} // namespace detail

void InterfacesTestObject::modbusRegistersTest01()
{
    std::uint16_t data = 0xaabb;
    auto reversed = qToBigEndian(data);
    auto first = StdFunc::toByteArray(reversed);
    auto second = Modbus::packRegister(data);
    QCOMPARE(first, second);
}

void InterfacesTestObject::modbusRegistersTest02()
{
    std::uint32_t data = 0xaabbccdd;
    auto first = StdFunc::toByteArray(qToBigEndian(data));
    auto second = Modbus::packRegister(data);
    auto third = detail::packReg(data);
    qDebug() << first << second << third;
    QVERIFY(first != second);
    QCOMPARE(second, third);
}

void InterfacesTestObject::modbusRegistersTest03()
{
    quint64 data = 0xaabbccddaabbccdd;
    auto first = StdFunc::toByteArray(qToBigEndian(data));
    auto second = Modbus::packRegister(data);
    auto third = detail::packReg(data);
    qDebug() << first << second << third;
    QVERIFY(first != second);
    QCOMPARE(second, third);
}

void InterfacesTestObject::modbusRegistersTest04()
{
    std::uint32_t data = 0xaabbccdd;
    auto first = StdFunc::toByteArray(data);
    auto second = Modbus::packRegister(data);
    auto third = Modbus::packRegister(first);
    qDebug() << first << second << third;
    QCOMPARE(second, third);
}

void InterfacesTestObject::modbusRegistersTest05()
{
    std::uint32_t data = 0xaabbccdd;
    auto packed = Modbus::packRegister(data);
    auto unpacked = detail::unpackReg<std::uint32_t>(packed);
    QCOMPARE(data, unpacked);
}

void InterfacesTestObject::modbusRegistersTest06()
{
    std::uint32_t data = 0xaabbccdd;
    auto packed = Modbus::packRegister(data);
    auto unpacked = Modbus::unpackRegister<std::uint32_t>(packed);
    QCOMPARE(data, unpacked);
}

void InterfacesTestObject::modbusRegistersTest07()
{
    std::uint32_t data = 0xaabbccdd;
    auto packed = Modbus::packRegister(data);
    auto first = detail::unpackReg<std::uint32_t>(packed);
    auto second = Modbus::unpackRegister<std::uint32_t>(packed);
    QCOMPARE(first, second);
}

void InterfacesTestObject::modbusSinglePoint()
{
    std::uint16_t value = 0xaaff;
    auto data = StdFunc::toByteArray(qToBigEndian(value));
    const auto first = detail::processSinglePointSignals01(data, 1);
    const auto second = detail::processSinglePointSignals02(data, 1);
    QVERIFY(detail::compare(first, second));
}
