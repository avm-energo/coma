#include "interfaces_test_obj.h"

#include <interfaces/types/iec104/apci.h>
#include <interfaces/types/iec104/asdu.h>
#include <interfaces/types/iec104/asdu_unpacker.h>

void InterfacesTestObject::iec104ApciTest01()
{
    using namespace Iec104;
    ControlBlock ctrl { FrameFormat::Information, 1, 2 };
    APCI apci { ctrl, 4 };
    auto actual = apci.toByteArray();
    auto expected = QByteArrayLiteral("\x68\x08\x02\x00\x04\x00");
    QVERIFY(actual.has_value());
    QCOMPARE(actual.value(), expected);
    apci.m_ctrlBlock.m_format = FrameFormat::Supervisory;
    apci.m_ctrlBlock.m_received = 3;
    actual = apci.toByteArray();
    expected = QByteArrayLiteral("\x68\x08\x01\x00\x06\x00");
    QVERIFY(actual.has_value());
    QCOMPARE(actual.value(), expected);
    apci.m_ctrlBlock.m_format = FrameFormat::Unnumbered;
    apci.m_ctrlBlock.m_func = ControlFunc::StopDataTransfer;
    apci.m_ctrlBlock.m_arg = ControlArg::Confirm;
    apci.m_asduSize = 249;
    actual = apci.toByteArray();
    expected = QByteArrayLiteral("\x68\xfd\x23\x00\x00\x00");
    QVERIFY(actual.has_value());
    QCOMPARE(actual.value(), expected);
}

void InterfacesTestObject::iec104ApciTest02()
{
    using namespace Iec104;
    auto actual = APCI::fromByteArray(QByteArrayLiteral("\x68\x08\x02\x00\x04\x00"));
    QVERIFY(actual.has_value());
    QCOMPARE(actual->m_asduSize, 4);
    QCOMPARE(actual->m_ctrlBlock.m_format, FrameFormat::Information);
    QCOMPARE(actual->m_ctrlBlock.m_received, 2);
    QCOMPARE(actual->m_ctrlBlock.m_sent, 1);
    actual = APCI::fromByteArray(QByteArrayLiteral("\x68\x04\x0b\x00\x00\x00"));
    QVERIFY(actual.has_value());
    QCOMPARE(actual->m_asduSize, 0);
    QCOMPARE(actual->m_ctrlBlock.m_format, FrameFormat::Unnumbered);
    QCOMPARE(actual->m_ctrlBlock.m_func, ControlFunc::StartDataTransfer);
    QCOMPARE(actual->m_ctrlBlock.m_arg, ControlArg::Confirm);
    actual = APCI::fromByteArray(QByteArrayLiteral("\x67\x04\x0b\x00\x00\x00"));
    QVERIFY(!actual.has_value());
    QCOMPARE(actual.error(), ApciError::InvalidStartByte);
    actual = APCI::fromByteArray(QByteArrayLiteral("\x68\x02\x0b\x00"));
    QVERIFY(!actual.has_value());
    QCOMPARE(actual.error(), ApciError::InvalidDataLength);
    actual = APCI::fromByteArray(QByteArrayLiteral("\x68\x04\xff\xff\xff\xff"));
    QVERIFY(!actual.has_value());
    QCOMPARE(actual.error(), ApciError::InvalidControlValue);
    actual = APCI::fromByteArray(QByteArrayLiteral("\x68\x04\x01\x01\x06\x00"));
    QVERIFY(!actual.has_value());
    QCOMPARE(actual.error(), ApciError::InvalidFrameFormat);
}

void InterfacesTestObject::iec104AsduTest01()
{
    using namespace Iec104;
    auto actual = ASDU::fromByteArray(QByteArrayLiteral("\x64\x01\x06\x00\xCD\x00\x00\x00\x00\x14"));
    QCOMPARE(actual.m_msgType, MessageDataType::C_IC_NA_1);
    QCOMPARE(actual.m_qualifier, StructureQualifier::Sequence);
    QCOMPARE(actual.m_elements, 1);
    QCOMPARE(actual.m_confirmation, Confirmation::Positive);
    QCOMPARE(actual.m_isTest, false);
    QCOMPARE(actual.m_cause, CauseOfTransmission::Activation);
    QCOMPARE(actual.m_originatorAddr, 0);
    QCOMPARE(actual.m_bsAddress, 205);
    QCOMPARE(actual.m_data, QByteArrayLiteral("\x00\x00\x00\x14"));
    actual = ASDU::fromByteArray(QByteArrayLiteral("\x65\x01\x0A\x00\x0C\x00\x00\x00\x00\x05"));
    QCOMPARE(actual.m_msgType, MessageDataType::C_CI_NA_1);
    QCOMPARE(actual.m_qualifier, StructureQualifier::Sequence);
    QCOMPARE(actual.m_elements, 1);
    QCOMPARE(actual.m_cause, CauseOfTransmission::ActivationTermination);
    QCOMPARE(actual.m_originatorAddr, 0);
    QCOMPARE(actual.m_bsAddress, 12);
    QCOMPARE(actual.m_data, QByteArrayLiteral("\x00\x00\x00\x05"));
    actual = ASDU::fromByteArray(QByteArrayLiteral("\x0B\x07\x03\x00\x0C\x00\x10\x30\x00\xBE\x09\x00"));
    QCOMPARE(actual.m_msgType, MessageDataType::M_ME_NB_1);
    QCOMPARE(actual.m_qualifier, StructureQualifier::Sequence);
    QCOMPARE(actual.m_elements, 7);
    QCOMPARE(actual.m_cause, CauseOfTransmission::Spontaneous);
    QCOMPARE(actual.m_originatorAddr, 0);
    QCOMPARE(actual.m_bsAddress, 12);
    QCOMPARE(actual.m_data, QByteArrayLiteral("\x10\x30\x00\xBE\x09\x00"));
}

void InterfacesTestObject::iec104AsduUnpackerTest01()
{
    using namespace Iec104;
    // Emulate BSI response from KIV
    auto response = QByteArrayLiteral(                                                                         //
        "\x68\x82\x02\x00\x02\x00\x07\x0f\x15\x00\xcd\x00\x01\x00\x00\xa2\x00\x00\x00\x00\x02\x00\x00\x84\x00" //
        "\x00\x00\x00\x03\x00\x00\x00\x00\x01\x02\x00\x04\x00\x00\x00\x00\x03\x03\x00\x05\x00\x00\x00\x00\x00" //
        "\x00\x00\x06\x00\x00\x8f\x03\x00\x00\x00\x07\x00\x00\x40\x00\x33\x00\x00\x08\x00\x00\x11\x47\x39\x33" //
        "\x00\x09\x00\x00\x38\x35\x32\x34\x00\x0a\x00\x00\x30\x60\x53\x21\x00\x0b\x00\x00\x24\x90\x51\x22\x00" //
        "\x0c\x00\x00\x00\x00\x01\x04\x00\x0d\x00\x00\x28\x30\x22\x22\x00\x0e\x00\x00\xc2\x61\x15\x6d\x00\x0f" //
        "\x00\x00\x40\x00\x00\x00\x00"                                                                         //
    );                                                                                                         //
    auto apciProduct = APCI::fromByteArray(response.left(apciSize));
    QVERIFY(apciProduct.has_value());
    auto apci = apciProduct.value();
    auto asdu = ASDU::fromByteArray(response.mid(apciSize, apci.m_asduSize));
    QVERIFY(!asdu.m_data.isEmpty());
    ASDUUnpacker unpacker;
    QSignalSpy spy(&unpacker, &ASDUUnpacker::unpacked);
    unpacker.unpack(asdu);
    std::uint32_t expectedAddress = 1;
    for (const auto &item : qAsConst(spy))
    {
        QVERIFY(item.at(0).canConvert<Interface::DeviceResponse>());
        auto value { item.at(0).value<Interface::DeviceResponse>() };
        QVERIFY(std::holds_alternative<DataTypes::BitStringStruct>(value));
        auto bitstring { std::get<DataTypes::BitStringStruct>(value) };
        auto actualAddress = bitstring.sigAdr;
        QCOMPARE(actualAddress, expectedAddress);
        ++expectedAddress;
    }
}
