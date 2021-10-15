#include "testmodule.h"

#include "../../gen/board.h"
#include "../../interfaces/modbusprivate.h"
#include "../../interfaces/protocomprivate.h"

TestModule::TestModule(QObject *parent) : QObject(parent)
{
}

void TestModule::checkA284()
{
    QString str("KIV");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xA2;
    bsi.MTypeM = 0x84;
    QVERIFY(module.loadSettings(str, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.size(), 48);
    QCOMPARE(settings->highlightCrit.size(), 7);
    QCOMPARE(settings->highlightWarn.size(), 30);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::checkA284USB()
{
    QString str("KIV");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xA2;
    bsi.MTypeM = 0x84;
    QVERIFY(module.loadSettings(str, bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.size(), 48);
    QCOMPARE(settings->highlightCrit.size(), 7);
    QCOMPARE(settings->highlightWarn.size(), 30);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 8);
    QCOMPARE(st.groups().size(), 8);
}

void TestModule::checkA284Eth()
{
    QString str("KIV");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xA2;
    bsi.MTypeM = 0x84;
    QVERIFY(module.loadSettings(str, bsi, Board::InterfaceType::Ethernet));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.size(), 48);
    QCOMPARE(settings->highlightCrit.size(), 7);
    QCOMPARE(settings->highlightWarn.size(), 30);
    QCOMPARE(settings->journals.size(), 2);
}

void TestModule::checkA284Modbus()
{
    QString str("KIV");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xA2;
    bsi.MTypeM = 0x84;
    QVERIFY(module.loadSettings(str, bsi, Board::InterfaceType::RS485));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.size(), 48);
    QCOMPARE(settings->highlightCrit.size(), 7);
    QCOMPARE(settings->highlightWarn.size(), 30);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<CommandsMBS::ModbusGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<CommandsMBS::ModbusGroup>>();
    QCOMPARE(st.dictionary().size(), 15);
    QCOMPARE(st.groups().size(), 15);
    QCOMPARE(st.regs().size(), 2);
    QCOMPARE(st.dictionaryRegs().size(), 2);
}

void TestModule::check8084()
{
    QString str("АВ-ТУК-8084");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x84;
    QVERIFY(module.loadSettings(str, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.size(), 39);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check8084USB()
{
    QString str("АВ-ТУК-8084");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x84;
    QVERIFY(module.loadSettings(str, bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.size(), 39);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 8);
    QCOMPARE(st.groups().size(), 8);
    QCOMPARE(st.regs().size(), 0);
    QCOMPARE(st.dictionaryRegs().size(), 0);
}

void TestModule::check8085()
{
    QString str("АВ-ТУК-8585 (УСК)");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x85;
    QVERIFY(module.loadSettings(str, bsi));
    auto settings = module.settings();
    QVERIFY(!settings->ifaceSettings.settings.isValid());
    // TODO Add other
}

void TestModule::check8085USB()
{
    QString str("АВ-ТУК-8585 (УСК)");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x85;
    QVERIFY(module.loadSettings(str, bsi, Board::InterfaceType::USB));
    //  auto settings = module.settings();
    // QVERIFY(settings->ifaceSettings.settings.isValid());
}

void TestModule::check8082()
{
    QString str("АВ-ТУК-8082 (Э1Т1Н)");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x82;
    QVERIFY(module.loadSettings(str, bsi));
    auto settings = module.settings();
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check8082USB()
{
    QString str("АВ-ТУК-8082 (Э1Т1Н)");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x80;
    bsi.MTypeM = 0x82;
    QVERIFY(module.loadSettings(str, bsi, Board::InterfaceType::USB));
    //  auto settings = module.settings();
    // QVERIFY(settings->ifaceSettings.settings.isValid());
}

void TestModule::check3533()
{
    QString str("АВ-ТУК-3533");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x33;
    QVERIFY(module.loadSettings(str, bsi));
    auto settings = module.settings();
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3533USB()
{
    QString str("АВ-ТУК-3533");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x33;
    QVERIFY(module.loadSettings(str, bsi, Board::InterfaceType::USB));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 2);
    QCOMPARE(st.groups().size(), 2);
}

QTEST_GUILESS_MAIN(TestModule)
