#include "testmodule.h"

//#include "../../module/board.h"
#include "testdata.h"

#include <QtXml>
#include <device/bsi.h>
#include <gen/stdfunc.h>
#include <interfaces/types/modbus_types.h>
#include <interfaces/types/protocom_types.h>
#include <s2/s2datamanager.h>

TestModule::TestModule(QObject *parent)
    : QObject(parent)
    , m_storage(new Device::ConfigStorage(this))
    , m_s2manager(new S2::DataManager(this))
    , m_xmlParser(new Xml::ModuleParser(this))
    , m_parseStatus(true)
{
}

int TestModule::getGroupsCount(const SectionList &list)
{
    auto groupCount = std::accumulate(list.cbegin(), list.cend(), 0, //
        [](int value, const Section &section)                        //
        {                                                            //
            return value + section.sgMap.count();                    //
        });                                                          //
    return groupCount;                                               //
}

std::size_t TestModule::getWidgetsCount(const SectionList &list)
{
    auto widgetCount = std::accumulate(list.cbegin(), list.cend(), 0ll,      //
        [](std::size_t value, const Section &section)                        //
        {                                                                    //
            const auto &map = section.sgMap;                                 //
            auto innerCount = std::accumulate(map.cbegin(), map.cend(), 0ll, //
                [](std::size_t inner, auto &&group) {                        //
                    return inner + group.widgets.size();                     //
                });                                                          //
            return value + innerCount;                                       //
        });                                                                  //
    return widgetCount;                                                      //
}

int TestModule::getAlarmsCount(const AlarmMap &map)
{
    auto alarmCount = std::accumulate(map.cbegin(), map.cend(), 0, //
        [](int count, auto &&alarm)                                //
        {                                                          //
            return count + alarm.size();                           //
        });                                                        //
    return alarmCount;                                             //
}

void TestModule::createInterfaceContext(const Interface::IfaceType &ifaceType)
{
    //    Connection::InterfacePointer device;
    //    switch (ifaceType)
    //    {
    //    case Interface::IfaceType::USB:
    //        device = Connection::InterfacePointer(new Protocom());
    //        break;
    //    case Interface::IfaceType::Ethernet:
    //        device = Connection::InterfacePointer(new IEC104());
    //        break;
    //    case Interface::IfaceType::RS485:
    //        device = Connection::InterfacePointer(new ModBus());
    //        break;
    //    default:
    //        device = nullptr;
    //        break;
    //    }
    //    Connection::setIface(std::move(device));
    //    Board::GetInstance().setInterfaceType(ifaceType);
    Q_UNUSED(ifaceType);
}

void TestModule::initTestCase()
{
    QObject::connect(m_xmlParser, &Xml::ModuleParser::startNewConfig,       //
        m_s2manager, &S2::DataManager::startNewConfig);                     //
    QObject::connect(m_xmlParser, &Xml::ModuleParser::configDataSending,    //
        m_s2manager, &S2::DataManager::configDataReceive);                  //
    QObject::connect(m_xmlParser, &Xml::ModuleParser::configNameSending,    //
        m_s2manager, &S2::DataManager::configNameReceive);                  //
    QObject::connect(m_xmlParser, &Xml::ModuleParser::signalDataSending,    //
        m_storage, &Device::ConfigStorage::signalDataReceive);              //
    QObject::connect(m_xmlParser, &Xml::ModuleParser::tabDataSending,       //
        m_storage, &Device::ConfigStorage::tabDataReceive);                 //
    QObject::connect(m_xmlParser, &Xml::ModuleParser::sectionDataSending,   //
        m_storage, &Device::ConfigStorage::sectionDataReceive);             //
    QObject::connect(m_xmlParser, &Xml::ModuleParser::alarmDataSending,     //
        m_storage, &Device::ConfigStorage::alarmDataReceive);               //
    QObject::connect(m_xmlParser, &Xml::ModuleParser::workJourDataSending,  //
        m_storage, &Device::ConfigStorage::workJourDataReceive);            //
    QObject::connect(m_xmlParser, &Xml::ModuleParser::measJourDataSending,  //
        m_storage, &Device::ConfigStorage::measJourDataReceive);            //
    QObject::connect(m_xmlParser, &Xml::ModuleParser::protocolGroupSending, //
        m_storage, &Device::ConfigStorage::protocolGroupReceive);           //
    QObject::connect(m_xmlParser, &Xml::ModuleParser::hiddenTabDataSending, //
        m_storage, &Device::ConfigStorage::hiddenTabDataReceive);           //

    QObject::connect(m_xmlParser, &Xml::ModuleParser::parseError, //
        this, [this](const QString &err) {
            qCritical() << err;
            m_parseStatus = false;
        });
}

void TestModule::cleanupTestCase()
{
    QDir homeDir(StdFunc::GetSystemHomeDir());
    auto xmlFiles = homeDir.entryList(QDir::Files).filter(".xml");
    for (auto &filename : xmlFiles)
    {
        QVERIFY(QFile::remove(homeDir.filePath(filename)));
    }
}

void TestModule::checkA284()
{
//    Device::BlockStartupInfo bsi = { 0xA2, 0x84, 0, StdFunc::StrToVer(a284::version) };
//    m_xmlParser->parse(bsi.MTypeB, bsi.MTypeM);
//    auto &settings = m_storage->getDeviceSettings();
//    // Journals comparing
//    auto workJourSize = settings.getWorkJours().size();
//    QCOMPARE(workJourSize, a284::workJours);
//    auto measJourSize = settings.getMeasJours().size();
//    QCOMPARE(measJourSize, a284::measJours);
//    // Alarms comparing
//    auto alarmsCount = getAlarmsCount(settings.getAlarms());
//    QCOMPARE(alarmsCount, a284::alarms);
//    // Highlights
//    QCOMPARE(settings.getHighlights(AlarmType::Critical).size(), a284::critHighlights);
//    QCOMPARE(settings.getHighlights(AlarmType::Warning).size(), a284::warnHighlights);
//    // Sections comparing
//    const auto &sections = settings.getSections();
//    QCOMPARE(sections.size(), a284::sections);
//    auto groupsCount = getGroupsCount(sections);
//    QCOMPARE(groupsCount, a284::sgroups);
//    auto widgetsCount = getWidgetsCount(sections);
//    QCOMPARE(widgetsCount, a284::mwidgets);
}

void TestModule::checkA284USB()
{
//    Device::BlockStartupInfo bsi = { 0xA2, 0x84, 0, StdFunc::StrToVer(a284::version) };
//    m_xmlParser->parse(bsi.MTypeB, bsi.MTypeM);
//    QVERIFY(m_parseStatus);
}

void TestModule::checkA284Eth()
{
//    Device::BlockStartupInfo bsi = { 0xA2, 0x84, 0, StdFunc::StrToVer(a284::version) };
//    m_xmlParser->parse(bsi.MTypeB, bsi.MTypeM);
//    QVERIFY(m_parseStatus);
}

void TestModule::checkA284Modbus()
{
//    Device::BlockStartupInfo bsi = { 0xA2, 0x84, 0, StdFunc::StrToVer(a284::version) };
//    m_xmlParser->parse(bsi.MTypeB, bsi.MTypeM);
//    QVERIFY(m_parseStatus);
}

/*
void TestModule::checkA287()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xa2;
    bsi.MTypeM = 0x87;
    bsi.Fwver = StdFunc::StrToVer(version::avma287);
    QVERIFY(module.loadSettings(bsi));
    auto settings = module.settings();
    QCOMPARE(settings->alarms.size(), 2);
    QCOMPARE(settings->configSettings.general.size(), config::avma287);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    auto workJourSize = settings->journals.value(Modules::JournalType::Work).desc.size();
    auto alarmsCount = std::accumulate(settings->alarms.cbegin(), settings->alarms.cend(), 0,
        [](int a, auto &&alarm) { return a + alarm.desc.size(); });
    QVERIFY(!settings->ifaceSettings.settings.isValid());
    QCOMPARE(workJourSize, alarmsCount);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 3);
    auto elementCount = getElementCount(checkSettings);
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
    QCOMPARE(settings->configSettings.general.size(), config::avma287);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 18);
    QCOMPARE(st.groups().size(), 18);

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 3);
    auto elementCount = getElementCount(checkSettings);
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
    QCOMPARE(settings->configSettings.general.size(), config::avma287);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 3);
    auto elementCount = getElementCount(checkSettings);
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
    QCOMPARE(settings->configSettings.general.size(), config::avma387);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    auto workJourSize = settings->journals.value(Modules::JournalType::Work).desc.size();
    auto alarmsCount = std::accumulate(settings->alarms.cbegin(), settings->alarms.cend(), 0,
        [](int a, auto &&alarm) { return a + alarm.desc.size(); });
    QVERIFY(!settings->ifaceSettings.settings.isValid());
    QCOMPARE(workJourSize, alarmsCount);
    QVERIFY(!settings->ifaceSettings.settings.isValid());

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 4);
    auto elementCount = getElementCount(checkSettings);
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
    QCOMPARE(settings->configSettings.general.size(), config::avma387);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.isValid());
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 20);
    QCOMPARE(st.groups().size(), 20);

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 4);
    auto elementCount = getElementCount(checkSettings);
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
    QCOMPARE(settings->configSettings.general.size(), config::avma387);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 4);
    auto elementCount = getElementCount(checkSettings);
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = getElementCount(checkSettings);
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
    QCOMPARE(settings->configSettings.general.size(), config::avtuk8084);
    QCOMPARE(settings->highlightCrit.size(), 0);
    QCOMPARE(settings->highlightWarn.size(), 0);
    QCOMPARE(settings->journals.size(), 2);
    QVERIFY(settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>());
    const auto &st = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
    QCOMPARE(st.dictionary().size(), 8);
    QCOMPARE(st.groups().size(), 8);
    QCOMPARE(st.regs().size(), 0);
    QCOMPARE(st.dictionaryRegs().size(), 0);

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = getElementCount(checkSettings);
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = getElementCount(checkSettings);
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = getElementCount(checkSettings);
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = getElementCount(checkSettings);
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 1);
    auto elementCount = getElementCount(checkSettings);
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = getElementCount(checkSettings);
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = getElementCount(checkSettings);
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 3);
    auto elementCount = getElementCount(checkSettings);
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 3);
    auto elementCount = getElementCount(checkSettings);
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = getElementCount(checkSettings);
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = getElementCount(checkSettings);
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
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

    std::vector<CheckItem> checkSettings;
    QVERIFY(
        module.loadCheckSettings(Modules::BaseBoard(bsi.MTypeB), Modules::MezzanineBoard(bsi.MTypeM), checkSettings));
    QCOMPARE(checkSettings.size(), 2);
    auto elementCount = std::accumulate(checkSettings.cbegin(), checkSettings.cend(), 0,
        [](auto value, const CheckItem &container) { return value + container.itemsVector.size(); });
    QCOMPARE(elementCount, 2);
}*/

QTEST_GUILESS_MAIN(TestModule)
