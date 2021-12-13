#include "testmodule.h"

#include "../../gen/board.h"
#include "../../interfaces/modbusprivate.h"
#include "../../interfaces/protocomprivate.h"

namespace config
{

constexpr auto avtuk21 = 17;
constexpr auto avtuk22 = 11;
constexpr auto avtuk31 = 5;
constexpr auto avtuk33 = 3;
constexpr auto avtuk34 = 7;
constexpr auto avtuk35 = 7;
constexpr auto avtuk8084 = 39;
constexpr auto avtuk8600 = 36;
constexpr auto avma284 = 48;
constexpr auto avtukSame = 0;
}

namespace check
{
constexpr auto categories = 5;
constexpr auto checkBase = 1;
constexpr auto check21 = 5;
constexpr auto check22 = 2;
constexpr auto check31 = 1;
constexpr auto check33 = 1;
constexpr auto check34 = 0;
constexpr auto check35 = 0;
constexpr auto check8081 = 10;
constexpr auto check8082 = 13;
constexpr auto check8083 = 12;
constexpr auto check8084 = 7;
constexpr auto check8585 = 2;
constexpr auto check8600 = 6;
}

namespace version
{
constexpr auto usio = "3.2-0002";
constexpr auto avtuk8x = "2.5-0002";
constexpr auto avtuk8585 = "3.8-0001";
constexpr auto avtuk8084 = "1.1-0015";
constexpr auto avma284 = "1.2-0005";
constexpr auto avtuk8600 = "1.0-0007";
constexpr auto avma287 = "0.6-0004";
constexpr auto avma387 = "0.10-0002";
}

TestModule::TestModule(QObject *parent) : QObject(parent)
{
}

void TestModule::checkA284()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xA2;
    bsi.MTypeM = 0x84;
    bsi.Fwver = StdFunc::StrToVer(version::avma284);
    QVERIFY(module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), config::avma284);
    QCOMPARE(settings->highlightCrit.size(), 7);
    QCOMPARE(settings->highlightWarn.size(), 30);
    QCOMPARE(settings->journals.size(), 2);
    auto workJourSize = settings->journals.value(Modules::JournalType::Work).desc.size();
    auto alarmsCount = std::accumulate(settings->alarms.cbegin(), settings->alarms.cend(), 0,
        [](int a, auto &&alarm) { return a + alarm.desc.size(); });
    QVERIFY(!settings->ifaceSettings.settings.isValid());
    QCOMPARE(workJourSize, alarmsCount);

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8084);
}

void TestModule::checkA284USB()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xA2;
    bsi.MTypeM = 0x84;
    bsi.Fwver = StdFunc::StrToVer(version::avma284);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), config::avma284);
    QCOMPARE(settings->highlightCrit.size(), 7);
    QCOMPARE(settings->highlightWarn.size(), 30);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 8);
    QCOMPARE(st.groups().size(), 8);

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8084);
}

void TestModule::checkA284Eth()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xA2;
    bsi.MTypeM = 0x84;
    bsi.Fwver = StdFunc::StrToVer(version::avma284);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::Ethernet));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), config::avma284);
    QCOMPARE(settings->highlightCrit.size(), 7);
    QCOMPARE(settings->highlightWarn.size(), 30);
    QCOMPARE(settings->journals.size(), 2);

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8084);
}

void TestModule::checkA284Modbus()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xA2;
    bsi.MTypeM = 0x84;
    bsi.Fwver = StdFunc::StrToVer(version::avma284);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::RS485));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), config::avma284);
    QCOMPARE(settings->highlightCrit.size(), 7);
    QCOMPARE(settings->highlightWarn.size(), 30);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<CommandsMBS::ModbusGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<CommandsMBS::ModbusGroup>>();
    QCOMPARE(st.dictionary().size(), 15);
    QCOMPARE(st.groups().size(), 15);
    QCOMPARE(st.regs().size(), 2);
    QCOMPARE(st.dictionaryRegs().size(), 2);

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8084);
}

void TestModule::checkA287()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xa2;
    bsi.MTypeM = 0x87;
    bsi.Fwver = StdFunc::StrToVer(version::avma287);
    QVERIFY(module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), 53);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    auto workJourSize = settings->journals.value(Modules::JournalType::Work).desc.size();
    auto alarmsCount = std::accumulate(settings->alarms.cbegin(), settings->alarms.cend(), 0,
        [](int a, auto &&alarm) { return a + alarm.desc.size(); });
    QVERIFY(!settings->ifaceSettings.settings.isValid());
    QCOMPARE(workJourSize, alarmsCount);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 3);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, 17);
}

void TestModule::checkA287USB()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xa2;
    bsi.MTypeM = 0x87;
    bsi.Fwver = StdFunc::StrToVer(version::avma287);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), 53);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 18);
    QCOMPARE(st.groups().size(), 18);

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 3);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, 17);
}

void TestModule::checkA287Eth()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xA2;
    bsi.MTypeM = 0x87;
    bsi.Fwver = StdFunc::StrToVer(version::avma287);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::Ethernet));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), 53);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 3);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, 17);
}

void TestModule::checkA387()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xa3;
    bsi.MTypeM = 0x87;
    bsi.Fwver = StdFunc::StrToVer(version::avma387);
    QVERIFY(module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), 73);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    auto workJourSize = settings->journals.value(Modules::JournalType::Work).desc.size();
    auto alarmsCount = std::accumulate(settings->alarms.cbegin(), settings->alarms.cend(), 0,
        [](int a, auto &&alarm) { return a + alarm.desc.size(); });
    QVERIFY(!settings->ifaceSettings.settings.isValid());
    QCOMPARE(workJourSize, alarmsCount);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 4);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, 23);
}

void TestModule::checkA387USB()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xa3;
    bsi.MTypeM = 0x87;
    bsi.Fwver = StdFunc::StrToVer(version::avma387);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), 73);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 20);
    QCOMPARE(st.groups().size(), 20);

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 4);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, 23);
}

void TestModule::checkA387Eth()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xA3;
    bsi.MTypeM = 0x87;
    bsi.Fwver = StdFunc::StrToVer(version::avma387);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::Ethernet));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), 73);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 4);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, 23);
}

void TestModule::check8084()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x84;
    bsi.Fwver = StdFunc::StrToVer(version::avtuk8084);
    QVERIFY(module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), config::avtuk8084);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8084);
}

void TestModule::check8084USB()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x84;
    bsi.Fwver = StdFunc::StrToVer(version::avtuk8084);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), 39);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 8);
    QCOMPARE(st.groups().size(), 8);
    QCOMPARE(st.regs().size(), 0);
    QCOMPARE(st.dictionaryRegs().size(), 0);

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8084);
}

void TestModule::check8585()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x85;
    bsi.MTypeM = 0x85;
    bsi.Fwver = StdFunc::StrToVer(version::avtuk8585);
    QVERIFY(module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8585);
}

void TestModule::check8585USB()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x85;
    bsi.MTypeM = 0x85;
    bsi.Fwver = StdFunc::StrToVer(version::avtuk8585);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 4);
    QCOMPARE(st.groups().size(), 4);
    QCOMPARE(st.regs().size(), 0);
    QCOMPARE(st.dictionaryRegs().size(), 0);

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8585);
}

void TestModule::check8600()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x86;
    bsi.MTypeM = 0x00;
    bsi.Fwver = StdFunc::StrToVer(version::avtuk8600);
    QVERIFY(module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), config::avtuk8600);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8600);
}

void TestModule::check8600USB()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x86;
    bsi.MTypeM = 0x00;
    bsi.Fwver = StdFunc::StrToVer(version::avtuk8600);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), config::avtuk8600);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 3);
    QCOMPARE(st.groups().size(), 3);
    QCOMPARE(st.regs().size(), 0);
    QCOMPARE(st.dictionaryRegs().size(), 0);

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8600);
}

void TestModule::check8081()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x81;
    bsi.Fwver = StdFunc::StrToVer(version::avtuk8x);
    QVERIFY(module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8081);
}

void TestModule::check8081USB()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x81;
    bsi.Fwver = StdFunc::StrToVer(version::avtuk8x);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8081);
}

void TestModule::check8082()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x82;
    bsi.Fwver = StdFunc::StrToVer(version::avtuk8x);
    QVERIFY(module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 3);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8082);
}

void TestModule::check8082USB()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x82;
    bsi.Fwver = StdFunc::StrToVer(version::avtuk8x);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 3);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8082);
}

void TestModule::check8083()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x83;
    bsi.Fwver = StdFunc::StrToVer(version::avtuk8x);
    QVERIFY(module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8083);
}

void TestModule::check8083USB()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x83;
    bsi.Fwver = StdFunc::StrToVer(version::avtuk8x);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::check8083);
}

void TestModule::check2100()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x00;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), 0);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check21);
}

void TestModule::check2121()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x21;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtukSame);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check21 + check::check21);
}

void TestModule::check2122()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x22;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk22);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check21 + check::check22);
}

void TestModule::check2131()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x31;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk31);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check21 + check::check31);
}

void TestModule::check2133()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x33;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk33);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check21 + check::check33);
}

void TestModule::check2134()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x34;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk34);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check21 + check::check34);
}

void TestModule::check2135()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x35;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk35);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check21 + check::check35);
}

void TestModule::check2200()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x00;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), 0);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check22);
}

void TestModule::check2221()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x21;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk21);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check22 + check::check21);
}

void TestModule::check2222()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x22;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtukSame);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check22 + check::check22);
}

void TestModule::check2231()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x31;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk31);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check22 + check::check31);
}

void TestModule::check2233()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x33;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk33);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check22 + check::check33);
}

void TestModule::check2234()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x34;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk34);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check22 + check::check34);
}

void TestModule::check2235()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x35;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk35);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check22 + check::check35);
}

void TestModule::check3100()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x00;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), 0);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check31);
}

void TestModule::check3121()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x21;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk21);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check31 + check::check21);
}

void TestModule::check3122()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x22;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk22);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check31 + check::check22);
}

void TestModule::check3131()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x31;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtukSame);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, 3);
}

void TestModule::check3133()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x33;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk33);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check31 + check::check33);
}

void TestModule::check3134()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x34;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk34);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check31 + check::check34);
}

void TestModule::check3135()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x35;
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk35);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check31 + check::check35);
}

void TestModule::check3300()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x00;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), 0);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check33);
}

void TestModule::check3321()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x21;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk21);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, 7);
}

void TestModule::check3322()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x22;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk22);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check33 + check::check22);
}

void TestModule::check3331()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x31;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk31);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check33 + check::check31);
}

void TestModule::check3333()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x33;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtukSame);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check33 + check::check33);
}

void TestModule::check3334()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x34;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk34);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check33 + check::check34);
}

void TestModule::check3335()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x35;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk35);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check33 + check::check35);
}

void TestModule::check3400()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x00;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    (module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), 0);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check34);
}

void TestModule::check3421()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x21;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk21);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check34 + check::check21);
}

void TestModule::check3422()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x22;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk22);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check34 + check::check22);
}

void TestModule::check3431()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x31;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk31);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check34 + check::check31);
}

void TestModule::check3433()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x33;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk33);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check34 + check::check33);
}

void TestModule::check3434()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x34;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    (module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtukSame);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check34 + check::check34);
}

void TestModule::check3435()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x35;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    (module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk35);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check34 + check::check35);
}

void TestModule::check3500()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x00;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    (module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), 0);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check35);
}

void TestModule::check3521()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x21;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk21);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check35 + check::check21);
}

void TestModule::check3522()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x22;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 1);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk22);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check35 + check::check22);
}

void TestModule::check3531()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x31;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk31);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check35 + check::check31);
}

void TestModule::check3533()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x33;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk33);
    QVERIFY(settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check35 + check::check33);
}

void TestModule::check3534()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x34;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    (module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk34);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check35 + check::check34);
}

void TestModule::check3535()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x35;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    (module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtukSame);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, check::checkBase + check::check35 + check::check35);
}

void TestModule::check3533USB()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x33;
    bsi.Fwver = StdFunc::StrToVer(version::usio);
    QVERIFY(module.loadSettings(bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 0);
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk33);
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 2);
    QCOMPARE(st.groups().size(), 2);

    auto checkSettings = module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, 2);
}

QTEST_GUILESS_MAIN(TestModule)
