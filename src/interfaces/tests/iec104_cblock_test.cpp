#include "interfaces_test_obj.h"

#include <gen/stdfunc.h>
#include <interfaces/types/iec104/control_block.h>

void InterfacesTestObject::iec104ControlBlockTest01()
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

void InterfacesTestObject::iec104ControlBlockTest02()
{
    using namespace Iec104;
    ControlBlock block { FrameFormat::Information, 1, 1 };
    auto data = block.data();
    QVERIFY(data.has_value());
    QCOMPARE(*data, 0x00020002);
    block.m_received = 3;
    block.m_sent = 2;
    data = block.data();
    QVERIFY(data.has_value());
    QCOMPARE(*data, 0x00060004);
}

void InterfacesTestObject::iec104ControlBlockTest03()
{
    using namespace Iec104;
    ControlBlock block { FrameFormat::Supervisory, 3, 3 };
    auto data = block.data();
    QVERIFY(data.has_value());
    QCOMPARE(*data, 0x00060001);
    block.m_received = 2;
    data = block.data();
    QVERIFY(data.has_value());
    QCOMPARE(*data, 0x00040001);
    block.m_received = 1;
    data = block.data();
    QVERIFY(data.has_value());
    QCOMPARE(*data, 0x00020001);
}

// Similar results as in IEC104Parser::SendS function
void InterfacesTestObject::iec104ControlBlockTest04()
{
    using namespace Iec104;
    ControlBlock block { FrameFormat::Supervisory, 0, 18321 };
    QByteArray first;
    first.append(static_cast<char>(0x01));
    first.append(static_cast<char>(0x00));
    first.append((block.m_received & 0x007F) << 1);
    first.append((block.m_received & 0x7F80) >> 7);

    auto data = block.data();
    QVERIFY(data.has_value());
    QCOMPARE(*data, 0x8F220001);
    auto second = StdFunc::toByteArray(*data);
    QCOMPARE(first, second);
}

void InterfacesTestObject::iec104UnnumberedControlTest01()
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

void InterfacesTestObject::iec104UnnumberedControlTest02()
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
