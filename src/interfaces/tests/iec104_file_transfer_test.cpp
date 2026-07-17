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
    // Section ready (секция 1, байт 5 - номер секции от устройства)
    feed(MessageDataType::F_SR_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x01"));
    // Segment "AB" (байт 6 - размер сегмента, данные с байта 7)
    feed(MessageDataType::F_SG_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x01\x02") + QByteArrayLiteral("AB"));
    // Last section, qualifier=3 (не финальная секция, байт 6 - qualifier)
    feed(MessageDataType::F_LS_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x01\x03"));
    // Section ready (секция 2)
    feed(MessageDataType::F_SR_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x02"));
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

// Регрессия на реальный кейс с устройства: F_LS(section=1, qualifier=3), затем ЕЩЁ РАЗ
// F_LS(section=1, qualifier=1) для ТОЙ ЖЕ секции (не следующей!) - ответ ConfirmFile должен
// эхом отдать section=1 из этого сообщения, а не собственный (инкрементированный) счётчик.
void InterfacesTestObject::iec104ResponseParserFileTransferSectionEchoTest01()
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

    auto feed = [&](const MessageDataType type, const QByteArray &data)
    {
        auto frame = makeDeviceFrame(type, 205, data, ctrl);
        parser.accumulateToResponseBuffer(frame);
        parser.parse();
    };

    feed(MessageDataType::F_FR_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x03\x00\x00"));
    feed(MessageDataType::F_SR_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x01"));
    feed(MessageDataType::F_SG_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x01\x03") + QByteArrayLiteral("XYZ"));
    feed(MessageDataType::F_LS_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x01\x03")); // section=1, qualifier=3
    feed(MessageDataType::F_LS_NA_1, QByteArrayLiteral("\x00\x00\x00\x01\x00\x01\x01")); // section=1 СНОВА, qualifier=1

    // 0=CallFile, 1=CallSection, 2=ConfirmSection, 3=ConfirmFile
    QCOMPARE(replySpy.count(), 4);
    QCOMPARE(replySpy.at(2).at(0).value<FileReplyAction>(), FileReplyAction::ConfirmSection);
    QCOMPARE(replySpy.at(2).at(2).value<quint8>(), quint8(1));
    QCOMPARE(replySpy.at(3).at(0).value<FileReplyAction>(), FileReplyAction::ConfirmFile);
    QCOMPARE(replySpy.at(3).at(2).value<quint8>(), quint8(1)); // не 2!
}

// Регрессия на краш из реального бэктрейса: ASDU::fromByteArray обращался к data[0..5]
// без проверки размера, что приводило к abort() в QByteArray::operator[] на пустом/битом ASDU.
void InterfacesTestObject::iec104AsduFromByteArrayShortDataTest01()
{
    using namespace Iec104;
    auto empty = ASDU::fromByteArray(QByteArray());
    QVERIFY(empty.m_data.isEmpty());

    auto shortData = ASDU::fromByteArray(QByteArrayLiteral("\x64\x01\x06"));
    QVERIFY(shortData.m_data.isEmpty());
}

// Тот же краш, но на уровне Iec104ResponseParser::parse(): I-кадр с нулевым ASDU
// (только 6-байтный APCI, без данных) раньше валился внутри parseInfoFormat.
void InterfacesTestObject::iec104ResponseParserEmptyAsduTest01()
{
    using namespace Interface;
    using namespace Iec104;

    Iec104ResponseParser parser;
    auto ctrl = std::make_shared<ControlBlock>(FrameFormat::Information, 0, 0);
    parser.updateControlBlock(ctrl);

    APCI apci(*ctrl, 0);
    apci.m_ctrlBlock.m_format = FrameFormat::Information;
    auto frame = apci.toByteArray();

    parser.accumulateToResponseBuffer(frame);
    parser.parse(); // не должно падать (abort()) - в этом весь тест
    QVERIFY(true);
}

void InterfacesTestObject::iec104AsduFileTransferDataWriteTest01()
{
    using namespace Iec104;
    ASDU asdu(205);
    // File ready: fileNum=3, размер файла=5 (section "съедает" младший байт размера)
    asdu.setFileTransferData(MessageDataType::F_FR_NA_1, 3, 0x05, QByteArrayLiteral("\x00\x00"));
    auto actual = asdu.toByteArray();
    auto expected = QByteArrayLiteral("\x78\x01\x0D\x00\xCD\x00\x00\x00\x00\x03\x00\x05\x00\x00");
    QCOMPARE(actual, expected);

    // Last section, qualifier=3, checksum=0x83
    asdu.setFileTransferData(MessageDataType::F_LS_NA_1, 1, 1, QByteArrayLiteral("\x03\x83"));
    actual = asdu.toByteArray();
    expected = QByteArrayLiteral("\x7B\x01\x0D\x00\xCD\x00\x00\x00\x00\x01\x00\x01\x03\x83");
    QCOMPARE(actual, expected);
}

void InterfacesTestObject::iec104RequestParserWriteBuildersTest01()
{
    using namespace Iec104;
    using namespace Interface;
    Iec104RequestParser parser;
    parser.setBaseStationAddress(205);
    auto ctrl = std::make_shared<ControlBlock>(FrameFormat::Information, 0, 0);
    parser.updateControlBlock(ctrl);
    const QByteArray apciAsduSize18 = QByteArrayLiteral("\x68\x12\x00\x00\x00\x00"); // asduSize=14
    const QByteArray apciAsduSize19 = QByteArrayLiteral("\x68\x13\x00\x00\x00\x00"); // asduSize=15

    // File ready: fileNum=3, размер=5
    auto actual = parser.createFileReadyMessage(3, 5);
    auto expected = apciAsduSize18 + QByteArrayLiteral("\x78\x01\x0D\x00\xCD\x00\x00\x00\x00\x03\x00\x05\x00\x00");
    QCOMPARE(actual, expected);

    // Section ready: размер=5
    actual = parser.createSectionReadyMessage(5);
    expected = apciAsduSize19 + QByteArrayLiteral("\x79\x01\x0D\x00\xCD\x00\x00\x00\x00\x01\x00\x01\x05\x00\x00");
    QCOMPARE(actual, expected);

    // Поток сегментов для payload "AB": 1 сегмент + завершающий F_LS(qualifier=3, checksum='A'+'B'=0x83)
    auto frames = parser.createSegmentBurst(QByteArrayLiteral("AB"));
    QCOMPARE(frames.size(), 2);
    QCOMPARE(frames.at(0), apciAsduSize19 + QByteArrayLiteral("\x7D\x01\x0D\x00\xCD\x00\x00\x00\x00\x01\x00\x01\x02\x41\x42"));
    QCOMPARE(frames.at(1), apciAsduSize18 + QByteArrayLiteral("\x7B\x01\x0D\x00\xCD\x00\x00\x00\x00\x01\x00\x01\x03\x83"));

    // Завершение файла: F_LS(qualifier=1, checksum=0x83)
    actual = parser.createFileFinalMessage(QByteArrayLiteral("AB"));
    expected = apciAsduSize18 + QByteArrayLiteral("\x7B\x01\x0D\x00\xCD\x00\x00\x00\x00\x01\x00\x01\x01\x83");
    QCOMPARE(actual, expected);
}

void InterfacesTestObject::iec104ResponseParserFileWriteTest01()
{
    using namespace Interface;
    using namespace Iec104;

    Iec104ResponseParser parser;
    auto ctrl = std::make_shared<ControlBlock>(FrameFormat::Information, 0, 0);
    parser.updateControlBlock(ctrl);
    const QByteArray payload = QByteArrayLiteral("AB");
    parser.setRequest(CommandStruct { Commands::C_WriteFile, quint32(3), payload });

    QSignalSpy writeReplySpy(&parser, &Iec104ResponseParser::fileWriteReplyNeeded);
    QSignalSpy responseSpy(&parser, &Iec104ResponseParser::responseParsed);
    QSignalSpy doneSpy(&parser, &Iec104ResponseParser::requestedDataReceived);

    auto feed = [&](const MessageDataType type, const quint8 qualifier)
    {
        auto data = QByteArrayLiteral("\x00\x00\x00\x00\x00\x00") + QByteArray(1, static_cast<char>(qualifier));
        auto frame = makeDeviceFrame(type, 205, data, ctrl);
        parser.accumulateToResponseBuffer(frame);
        parser.parse();
    };

    feed(MessageDataType::F_SC_NA_1, 0x02); // запрос файла (размера)
    feed(MessageDataType::F_SC_NA_1, 0x06); // запрос сегментов
    feed(MessageDataType::F_AF_NA_1, 0x03); // секция принята
    feed(MessageDataType::F_AF_NA_1, 0x01); // файл принят целиком

    QCOMPARE(writeReplySpy.count(), 3);
    const FileWriteReplyAction expectedActions[] = {
        FileWriteReplyAction::SectionReady,
        FileWriteReplyAction::SendSegments,
        FileWriteReplyAction::FileFinal,
    };
    for (int i = 0; i < writeReplySpy.count(); ++i)
    {
        QCOMPARE(writeReplySpy.at(i).at(0).value<FileWriteReplyAction>(), expectedActions[i]);
        QCOMPARE(writeReplySpy.at(i).at(1).value<QByteArray>(), payload);
    }

    QCOMPARE(doneSpy.count(), 1);

    bool foundOk = false;
    for (const auto &call : std::as_const(responseSpy))
    {
        auto value = call.at(0).value<Interface::DeviceResponse>();
        if (std::holds_alternative<DataTypes::GeneralResponseStruct>(value))
        {
            auto resp = std::get<DataTypes::GeneralResponseStruct>(value);
            if (resp.type == DataTypes::GeneralResponseTypes::Ok)
            {
                QCOMPARE(resp.data, quint64(payload.size()));
                foundOk = true;
            }
        }
    }
    QVERIFY(foundOk);
}
