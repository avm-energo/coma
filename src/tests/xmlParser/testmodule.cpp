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
constexpr auto avtukSame = 0;
}

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
    QCOMPARE(settings->configSettings.general.size(), 48);
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
    QCOMPARE(settings->configSettings.general.size(), 48);
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
    QCOMPARE(settings->configSettings.general.size(), 48);
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
    QCOMPARE(settings->configSettings.general.size(), 48);
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
    QCOMPARE(settings->configSettings.general.size(), 39);
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

void TestModule::check2100()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x00;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), 0);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2121()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x21;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtukSame);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2122()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x22;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk22);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2131()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x31;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk31);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2133()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x33;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk33);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2134()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x34;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk34);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2135()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x21;
    bsi.MTypeM = 0x35;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk21);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk35);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2200()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x00;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), 0);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2221()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x21;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk21);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2222()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x22;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtukSame);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2231()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x31;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk31);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2233()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x33;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk33);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2234()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x34;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk34);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check2235()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x22;
    bsi.MTypeM = 0x35;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk22);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk35);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3100()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x00;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), 0);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3121()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x21;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk21);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3122()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x22;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk22);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3131()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x31;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtukSame);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3133()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x33;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk33);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3134()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x34;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk34);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3135()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x31;
    bsi.MTypeM = 0x35;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk31);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk35);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3300()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x00;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), 0);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3321()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x21;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk21);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3322()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x22;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk22);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3331()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x31;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk31);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3333()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x33;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtukSame);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3334()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x34;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk34);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3335()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x33;
    bsi.MTypeM = 0x35;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk33);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk35);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3400()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x00;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), 0);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3421()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x21;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk21);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3422()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x22;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk22);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3431()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x31;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk31);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3433()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x33;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk33);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3434()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x34;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtukSame);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3435()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x34;
    bsi.MTypeM = 0x35;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk34);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk35);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3500()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x00;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), 0);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3521()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x21;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk21);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3522()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x22;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk22);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3531()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x31;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk31);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3533()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x33;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    QVERIFY(module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk33);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3534()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x34;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk34);
    QVERIFY(!settings->ifaceSettings.settings.isValid());
}

void TestModule::check3535()
{
    QString str("АВ-ТУК-%1%2");
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x35;
    bsi.MTypeM = 0x35;
    auto moduleName = str.arg(bsi.MTypeB, 0, 16).arg(bsi.MTypeM, 0, 16);
    (module.loadSettings(moduleName, bsi));
    auto settings = module.settings();
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtukSame);
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
    QCOMPARE(settings->configSettings.general.size(), 8);
    QCOMPARE(settings->configSettings.base.size(), config::avtuk35);
    QCOMPARE(settings->configSettings.mezz.size(), config::avtuk33);
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 2);
    QCOMPARE(st.groups().size(), 2);
}

QTEST_GUILESS_MAIN(TestModule)
