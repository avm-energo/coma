#include "interfaces_test_obj.h"

#include <interfaces/parsers/iec104_request_parser.h>
#include <interfaces/parsers/iec104_response_parser.h>
#include <interfaces/types/common_types.h>
#include <interfaces/types/iec104/apci.h>
#include <interfaces/types/iec104/asdu.h>
#include <libavm-gen/datatypes.h>
#include <s2/s2datatypes.h>

namespace
{
// Строит сырой кадр APCI+ASDU для файлового сообщения, приходящего от устройства.
QByteArray makeDeviceFrame(const Iec104::MessageDataType type, const quint16 bsAddress, //
    const QByteArray &asduData, const SharedControlBlock &ctrl)
{
    using namespace Iec104;
    ASDU asdu(bsAddress);
    asdu.m_msgType = type;
    asdu.m_qualifier = StructureQualifier::Sequence;
    asdu.m_elements = 1;
    asdu.m_cause = CauseOfTransmission::FileTransfering;
    asdu.m_confirmation = Confirmation::Positive;
    asdu.m_isTest = false;
    asdu.m_data = asduData;
    auto asduBytes = asdu.toByteArray();

    APCI apci(*ctrl, asduBytes.size());
    apci.m_ctrlBlock.m_format = FrameFormat::Information;
    auto frame = apci.toByteArray();
    frame.append(asduBytes);
    return frame;
}
} // namespace

void InterfacesTestObject::iec104AsduFileTransferDataTest01()
{
    using namespace Iec104;
    ASDU asdu(205);
    asdu.setFileTransferData(MessageDataType::F_SC_NA_1, 4, 0, 0x01); // Select file
    auto actual = asdu.toByteArray();
    auto expected = QByteArrayLiteral("\x7A\x01\x0D\x00\xCD\x00\x00\x00\x00\x04\x00\x00\x01");
    QCOMPARE(actual, expected);

    asdu.setFileTransferData(MessageDataType::F_SC_NA_1, 5, 3, 0x06); // Call section 3 of file 5
    actual = asdu.toByteArray();
    expected = QByteArrayLiteral("\x7A\x01\x0D\x00\xCD\x00\x00\x00\x00\x05\x00\x03\x06");
    QCOMPARE(actual, expected);

    asdu.setFileTransferData(MessageDataType::F_AF_NA_1, 4, 2, 0x01); // Confirm file 4, section 2
    actual = asdu.toByteArray();
    expected = QByteArrayLiteral("\x7C\x01\x0D\x00\xCD\x00\x00\x00\x00\x04\x00\x02\x01");
    QCOMPARE(actual, expected);
}

void InterfacesTestObject::iec104RequestParserFileReplyTest01()
{
    using namespace Iec104;
    using namespace Interface;
    Iec104RequestParser parser;
    parser.setBaseStationAddress(205);
    auto ctrl = std::make_shared<ControlBlock>(FrameFormat::Information, 1, 2);
    parser.updateControlBlock(ctrl);

    auto actual = parser.createFileReply(FileReplyAction::Select, 4, 0);
    auto expected = QByteArrayLiteral(
        "\x68\x11\x02\x00\x04\x00" // APCI: sent=1, received=2, asduSize=13
        "\x7A\x01\x0D\x00\xCD\x00\x00\x00\x00\x04\x00\x00\x01");
    QCOMPARE(actual, expected);
}

void InterfacesTestObject::iec104ResponseParserFileTransferTest01()
{
    using namespace Interface;
    using namespace Iec104;

    Iec104ResponseParser parser;
    auto ctrl = std::make_shared<ControlBlock>(FrameFormat::Information, 0, 0);
    parser.updateControlBlock(ctrl);
    constexpr quint8 fileNum = 1;
    parser.setRequest(CommandStruct { //
        Commands::C_ReqFile, quint32(fileNum), DataTypes::FileFormat::DefaultS2 });

    QSignalSpy replySpy(&parser, &Iec104ResponseParser::fileReplyNeeded);
    QSignalSpy responseSpy(&parser, &Iec104ResponseParser::responseParsed);
    QSignalSpy doneSpy(&parser, &Iec104ResponseParser::requestedDataReceived);

    auto feed = [&](const MessageDataType type, const QByteArray &data)
    {
        auto frame = makeDeviceFrame(type, 205, data, ctrl);
        parser.accumulateToResponseBuffer(frame);
        parser.parse();
    };

    // File ready, полный размер файла = 5 байт (две несекции: "AB" + "CDE")
    feed(MessageDataType::F_FR_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x05\x00\x00"));
    // Section ready (секция 1)
    feed(MessageDataType::F_SR_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00"));
    // Segment "AB" (байт 6 - размер сегмента, данные с байта 7)
    feed(MessageDataType::F_SG_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x01\x02") + QByteArrayLiteral("AB"));
    // Last section, qualifier=3 (не финальная секция, байт 6 - qualifier)
    feed(MessageDataType::F_LS_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x01\x03"));
    // Section ready (секция 2)
    feed(MessageDataType::F_SR_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00"));
    // Segment "CDE"
    feed(MessageDataType::F_SG_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x02\x03") + QByteArrayLiteral("CDE"));
    // Last section, qualifier=1 (файл получен целиком)
    feed(MessageDataType::F_LS_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x02\x01"));

    QCOMPARE(replySpy.count(), 5);
    struct Expected
    {
        FileReplyAction action;
        quint8 section;
    };
    const Expected expected[] = {
        { FileReplyAction::CallFile, 0 },
        { FileReplyAction::CallSection, 1 },
        { FileReplyAction::ConfirmSection, 1 },
        { FileReplyAction::CallSection, 2 },
        { FileReplyAction::ConfirmFile, 2 },
    };
    for (int i = 0; i < replySpy.count(); ++i)
    {
        QCOMPARE(replySpy.at(i).at(0).value<FileReplyAction>(), expected[i].action);
        QCOMPARE(replySpy.at(i).at(1).value<quint8>(), fileNum);
        QCOMPARE(replySpy.at(i).at(2).value<quint8>(), expected[i].section);
    }

    QCOMPARE(doneSpy.count(), 1);

    bool foundAssembledFile = false;
    for (const auto &call : std::as_const(responseSpy))
    {
        auto value = call.at(0).value<Interface::DeviceResponse>();
        if (std::holds_alternative<QByteArray>(value))
        {
            QCOMPARE(std::get<QByteArray>(value), QByteArrayLiteral("ABCDE"));
            foundAssembledFile = true;
        }
    }
    QVERIFY(foundAssembledFile);
}
