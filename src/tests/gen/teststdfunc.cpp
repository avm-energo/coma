#include "teststdfunc.h"

#include <array>
#include <gen/datatypes.h>
#include <gen/error.h>
#include <gen/stdfunc.h>
#include <interfaces/types/iec104/apci.h>
#include <interfaces/types/iec104/control_block.h>

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
inline T unpackRegister(const QByteArray &ba)
{
    assert(N == ba.size());
    T value;
    auto dstBegin = reinterpret_cast<std::uint8_t *>(&value);
    std::copy(ba.cbegin(), ba.cend(), dstBegin);
    for (auto i = 0; i < N; i += 2)
        std::swap(dstBegin[i], dstBegin[i + 1]);
    return value;
}

template <typename T, std::size_t N = sizeof(T)> //
inline QByteArray packRegister(T value)
{
    static_assert(N % 2 == 0, "The size of type T must be even");
    static_assert(N >= 2, "The size of type T must be greater than or equal to 2");
    QByteArray ba;
    ba.reserve(N);
    auto srcBegin = reinterpret_cast<std::uint8_t *>(&value);
    auto srcEnd = srcBegin + sizeof(T);
    for (auto it = srcBegin; it != srcEnd; it = it + 2)
    {
        ba.push_back(*(it + 1));
        ba.push_back(*it);
    }
    return ba;
}

inline QByteArray packRegister(const QByteArray &value)
{
    const auto N = value.size();
    Q_ASSERT(N % 2 == 0);
    Q_ASSERT(N >= 2);
    QByteArray ba;
    ba.reserve(value.size());
    for (auto it = value.cbegin(); it != value.cend(); it = it + 2)
    {
        ba.push_back(*(it + 1));
        ba.push_back(*it);
    }
    return ba;
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

TestStdFunc::TestStdFunc(QObject *parent) : QObject(parent)
{
}

void TestStdFunc::verToStr()
{
    QString expectedString("3.3-0001");
    auto realString = StdFunc::VerToStr(50528257);
    QCOMPARE(expectedString, realString);
}

void TestStdFunc::strToVer()
{
    quint32 expectedValue = 50528257;
    auto realValue = StdFunc::StrToVer("3.3-0001");
    QCOMPARE(expectedValue, realValue);
    QVERIFY(StdFunc::StrToVer("3.3-0001") > StdFunc::StrToVer("3.3-0000"));
    QVERIFY(StdFunc::StrToVer("4.3-0001") > StdFunc::StrToVer("3.3-0000"));
}

void TestStdFunc::enumToStr()
{
    auto err = Error::Msg::CrcError;
    auto metaEnum = QMetaEnum::fromType<decltype(err)>();
    QString errStr(metaEnum.valueToKey(err));
    QString expected = "CrcError";
    QCOMPARE(errStr, expected);
}

void TestStdFunc::byteArrayTest01()
{
    quint16 data = 32125;
    QByteArray first;
    first.append(static_cast<char>(data % 0x100));
    first.append(static_cast<char>(data / 0x100));
    auto second = StdFunc::toByteArray(data);
    QCOMPARE(first, second);
}

void TestStdFunc::byteArrayTest02()
{
    quint16 data = 12205;
    QByteArray first;
    first.append(static_cast<char>(data & 0xFF)); // low
    first.append(static_cast<char>(data >> 8));   // high
    auto second = StdFunc::toByteArray(data);
    QCOMPARE(first, second);
}

void TestStdFunc::modbusRegistersTest01()
{
    std::uint16_t data = 0xaabb;
    auto first = StdFunc::toByteArray(qToBigEndian(data));
    auto second = detail::packRegister(data);
    QCOMPARE(first, second);
}

void TestStdFunc::modbusRegistersTest02()
{
    std::uint32_t data = 0xaabbccdd;
    auto first = StdFunc::toByteArray(qToBigEndian(data));
    auto second = detail::packRegister(data);
    auto third = detail::packReg(data);
    qDebug() << first << second << third;
    QVERIFY(first != second);
    QCOMPARE(second, third);
}

void TestStdFunc::modbusRegistersTest03()
{
    quint64 data = 0xaabbccddaabbccdd;
    auto first = StdFunc::toByteArray(qToBigEndian(data));
    auto second = detail::packRegister(data);
    auto third = detail::packReg(data);
    qDebug() << first << second << third;
    QVERIFY(first != second);
    QCOMPARE(second, third);
}

void TestStdFunc::modbusRegistersTest04()
{
    std::uint32_t data = 0xaabbccdd;
    auto first = StdFunc::toByteArray(data);
    auto second = detail::packRegister(data);
    auto third = detail::packRegister(first);
    qDebug() << first << second << third;
    QCOMPARE(second, third);
}

void TestStdFunc::modbusRegistersTest05()
{
    std::uint32_t data = 0xaabbccdd;
    auto packed = detail::packRegister(data);
    auto unpacked = detail::unpackReg<std::uint32_t>(packed);
    QCOMPARE(data, unpacked);
}

void TestStdFunc::modbusRegistersTest06()
{
    std::uint32_t data = 0xaabbccdd;
    auto packed = detail::packRegister(data);
    auto unpacked = detail::unpackRegister<std::uint32_t>(packed);
    QCOMPARE(data, unpacked);
}

void TestStdFunc::modbusRegistersTest07()
{
    std::uint32_t data = 0xaabbccdd;
    auto packed = detail::packRegister(data);
    auto first = detail::unpackReg<std::uint32_t>(packed);
    auto second = detail::unpackRegister<std::uint32_t>(packed);
    QCOMPARE(first, second);
}

void TestStdFunc::modbusSinglePoint()
{
    std::uint16_t value = 0xaaff;
    auto data = StdFunc::toByteArray(qToBigEndian(value));
    const auto first = detail::processSinglePointSignals01(data, 1);
    const auto second = detail::processSinglePointSignals02(data, 1);
    QVERIFY(detail::compare(first, second));
}

void TestStdFunc::iec104ControlBlockTest01()
{
    using namespace Iec104;
    UnnumberedControl block {};
    QCOMPARE(block.startDataTransferActivate(), 0x07);
    QCOMPARE(block.startDataTransferActivate(), 0b00000111);
    QCOMPARE(block.startDataTransferConfirm(), 0x0B);
    QCOMPARE(block.startDataTransferConfirm(), 0b00001011);
    QCOMPARE(block.stopDataTransferActivate(), 0x13);
    QCOMPARE(block.stopDataTransferActivate(), 0b00010011);
    QCOMPARE(block.stopDataTransferConfirm(), 0x23);
    QCOMPARE(block.stopDataTransferConfirm(), 0b00100011);
    QCOMPARE(block.testFrameActivate(), 0x43);
    QCOMPARE(block.testFrameActivate(), 0b01000011);
    QCOMPARE(block.testFrameConfirm(), 0x83);
    QCOMPARE(block.testFrameConfirm(), 0b10000011);
}

void TestStdFunc::iec104ControlBlockTest02()
{
    using namespace Iec104;
    ControlBlock block { FrameFormat::Information, 1, 1 };
    auto data = block.data();
    QVERIFY(data.has_value());
    QCOMPARE(*data, 0x00020002);
    block.received = 3;
    block.sent = 2;
    data = block.data();
    QVERIFY(data.has_value());
    QCOMPARE(*data, 0x00060004);
}

void TestStdFunc::iec104ControlBlockTest03()
{
    using namespace Iec104;
    ControlBlock block { FrameFormat::Supervisory, 3, 3 };
    auto data = block.data();
    QVERIFY(data.has_value());
    QCOMPARE(*data, 0x00060001);
    block.received = 2;
    data = block.data();
    QVERIFY(data.has_value());
    QCOMPARE(*data, 0x00040001);
    block.received = 1;
    data = block.data();
    QVERIFY(data.has_value());
    QCOMPARE(*data, 0x00020001);
}

// Similar results as in IEC104Parser::SendS function
void TestStdFunc::iec104ControlBlockTest04()
{
    using namespace Iec104;
    ControlBlock block { FrameFormat::Supervisory, 0, 18321 };
    QByteArray first;
    first.append(static_cast<char>(0x01));
    first.append(static_cast<char>(0x00));
    first.append((block.received & 0x007F) << 1);
    first.append((block.received & 0x7F80) >> 7);

    auto data = block.data();
    QVERIFY(data.has_value());
    QCOMPARE(*data, 0x8F220001);
    auto second = StdFunc::toByteArray(*data);
    QCOMPARE(first, second);
}

void TestStdFunc::iec104UnnumberedControlTest01()
{
    using namespace Iec104;
    UnnumberedControl block {};
    auto actual = block.startDataTransferActivate();
    auto expected = (1 << 2) | 3;
    QCOMPARE(actual, expected);
    actual = block.startDataTransferConfirm();
    expected = (1 << 3) | 3;
    QCOMPARE(actual, expected);
    actual = block.stopDataTransferActivate();
    expected = (1 << 4) | 3;
    QCOMPARE(actual, expected);
    actual = block.stopDataTransferConfirm();
    expected = (1 << 5) | 3;
    QCOMPARE(actual, expected);
    actual = block.testFrameActivate();
    expected = (1 << 6) | 3;
    QCOMPARE(actual, expected);
    actual = block.testFrameConfirm();
    expected = (1 << 7) | 3;
    QCOMPARE(actual, expected);
}

void TestStdFunc::iec104UnnumberedControlTest02()
{
    using namespace Iec104;
    UnnumberedControl block {};
    auto value = block.getValue(static_cast<ControlFunc>(5), static_cast<ControlArg>(10));
    QVERIFY(!value.has_value());
    QCOMPARE(value.error(), ApciError::InvalidControlArg);
    value = block.getValue(static_cast<ControlFunc>(5), ControlArg::Confirm);
    QVERIFY(!value.has_value());
    QCOMPARE(value.error(), ApciError::InvalidControlFunc);
    value = block.getValue(ControlFunc::StartDataTransfer, ControlArg::Activate);
    QVERIFY(value.has_value());
    QCOMPARE(*value, block.startDataTransferActivate());
}

void TestStdFunc::iec104ApciTest01()
{
    using namespace Iec104;
    ControlBlock ctrl { FrameFormat::Information, 1, 2 };
    APCI apci { ctrl, 4 };
    auto actual = apci.toByteArray();
    auto expected = QByteArrayLiteral("\x68\x08\x02\x00\x04\x00");
    QVERIFY(actual.has_value());
    QCOMPARE(actual.value(), expected);
    ctrl.format = FrameFormat::Supervisory;
    ctrl.received = 3;
    apci.updateControlBlock(ctrl);
    actual = apci.toByteArray();
    expected = QByteArrayLiteral("\x68\x08\x01\x00\x06\x00");
    QVERIFY(actual.has_value());
    QCOMPARE(actual.value(), expected);
    ctrl.format = FrameFormat::Unnumbered;
    ctrl.func = ControlFunc::StopDataTransfer;
    ctrl.arg = ControlArg::Confirm;
    apci.updateControlBlock(ctrl);
    apci.updateASDUSize(249);
    actual = apci.toByteArray();
    expected = QByteArrayLiteral("\x68\xfd\x23\x00\x00\x00");
    QVERIFY(actual.has_value());
    QCOMPARE(actual.value(), expected);
}

QTEST_GUILESS_MAIN(TestStdFunc)
