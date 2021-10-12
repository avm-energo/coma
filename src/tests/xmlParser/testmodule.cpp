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
    QVERIFY(module.loadSettings(str, 0x84, 0xA2));
    auto settings = module.settings();
    QVERIFY(settings->alarms.size() == 2);
    QVERIFY(settings->configSettings.size() == 48);
    QVERIFY(settings->highlightCrit.size() == 7);
    QVERIFY(settings->highlightWarn.size() == 30);
    QVERIFY(settings->journals.size() == 2);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::checkA284USB()
{
    QString str("KIV");
    QVERIFY(module.loadSettings(str, 0x84, 0xA2, Board::InterfaceType::USB));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QVERIFY(settings->alarms.size() == 2);
    QVERIFY(settings->configSettings.size() == 48);
    QVERIFY(settings->highlightCrit.size() == 7);
    QVERIFY(settings->highlightWarn.size() == 30);
    QVERIFY(settings->journals.size() == 2);
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QVERIFY(st.dictionary().size() == 8);
    QVERIFY(st.groups().size() == 8);
}

void TestModule::checkA284Eth()
{
    QString str("KIV");
    QVERIFY(module.loadSettings(str, 0x84, 0xA2, Board::InterfaceType::Ethernet));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QVERIFY(settings->alarms.size() == 2);
    QVERIFY(settings->configSettings.size() == 48);
    QVERIFY(settings->highlightCrit.size() == 7);
    QVERIFY(settings->highlightWarn.size() == 30);
    QVERIFY(settings->journals.size() == 2);
}

void TestModule::checkA284Modbus()
{
    QString str("KIV");
    QVERIFY(module.loadSettings(str, 0x84, 0xA2, Board::InterfaceType::RS485));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QVERIFY(settings->alarms.size() == 2);
    QVERIFY(settings->configSettings.size() == 48);
    QVERIFY(settings->highlightCrit.size() == 7);
    QVERIFY(settings->highlightWarn.size() == 30);
    QVERIFY(settings->journals.size() == 2);
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<CommandsMBS::ModbusGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<CommandsMBS::ModbusGroup>>();
    QVERIFY(st.dictionary().size() == 15);
    QVERIFY(st.groups().size() == 15);
    QVERIFY(st.regs().size() == 2);
    QVERIFY(st.dictionaryRegs().size() == 2);
}

void TestModule::check8084()
{
    QString str("АВ-ТУК-8084");
    QVERIFY(module.loadSettings(str, 0x84, 0x80));
    auto settings = module.settings();
    QVERIFY(settings->alarms.size() == 2);
    QVERIFY(settings->configSettings.size() == 39);
    QVERIFY(settings->highlightCrit.size() == 0);
    QVERIFY(settings->highlightWarn.size() == 0);
    QVERIFY(settings->journals.size() == 2);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check8084USB()
{
    QString str("АВ-ТУК-8084");
    QVERIFY(module.loadSettings(str, 0x84, 0x80, Board::InterfaceType::USB));
    auto settings = module.settings();
    QVERIFY(settings->alarms.size() == 2);
    QVERIFY(settings->configSettings.size() == 39);
    QVERIFY(settings->highlightCrit.size() == 0);
    QVERIFY(settings->highlightWarn.size() == 0);
    QVERIFY(settings->journals.size() == 2);
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QVERIFY(st.dictionary().size() == 8);
    QVERIFY(st.groups().size() == 8);
    QVERIFY(st.regs().size() == 0);
    QVERIFY(st.dictionaryRegs().size() == 0);
}

void TestModule::check8085()
{
    QString str("АВ-ТУК-8585 (УСК)");
    QVERIFY(module.loadSettings(str, 0x85, 0x80));
    auto settings = module.settings();
    QVERIFY(!settings->ifaceSettings.settings.isValid());
    // TODO Add other
}

void TestModule::check8085USB()
{
    QString str("АВ-ТУК-8585 (УСК)");
    QVERIFY(module.loadSettings(str, 0x85, 0x80, Board::InterfaceType::USB));
    //  auto settings = module.settings();
    // QVERIFY(settings->ifaceSettings.settings.isValid());
}

void TestModule::check8082()
{
    QString str("АВ-ТУК-8082 (Э1Т1Н)");
    QVERIFY(module.loadSettings(str, 0x82, 0x80));
    auto settings = module.settings();
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check8082USB()
{
    QString str("АВ-ТУК-8082 (Э1Т1Н)");
    QVERIFY(module.loadSettings(str, 0x82, 0x80, Board::InterfaceType::USB));
    //  auto settings = module.settings();
    // QVERIFY(settings->ifaceSettings.settings.isValid());
}

void TestModule::check3533()
{
    QString str("АВ-ТУК-3533");
    QVERIFY(module.loadSettings(str, 0x33, 0x35));
    auto settings = module.settings();
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3533USB()
{
    QString str("АВ-ТУК-3533");
    QVERIFY(module.loadSettings(str, 0x33, 0x35, Board::InterfaceType::USB));
    auto settings = module.settings();
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QVERIFY(st.dictionary().size() == 2);
    QVERIFY(st.groups().size() == 2);
}

QTEST_GUILESS_MAIN(TestModule)
