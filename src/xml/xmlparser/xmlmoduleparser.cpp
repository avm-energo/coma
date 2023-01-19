#include "xmlmoduleparser.h"

#include <gen/stdfunc.h>

Xml::ModuleParser::ModuleParser(QObject *parent) : BaseParser(parent), isBase(true)
{
}

/// \brief Проверка на то, совпадает ли тип модуля с указанным в файле конфигурации.
bool Xml::ModuleParser::isCorrectModuleType(const QDomElement &moduleNode, const quint16 &typeB, const quint16 &typeM)
{
    if (moduleNode.hasAttribute(tags::mtypeb) && moduleNode.hasAttribute(tags::mtypem))
    {
        auto mTypeB = moduleNode.attribute(tags::mtypeb, "");
        auto mTypeM = moduleNode.attribute(tags::mtypem, "");
        bool status1 = false, status2 = false;
        quint16 mtypeB = mTypeB.toUShort(&status1, 16);
        quint16 mtypeM = mTypeM.toUShort(&status2, 16);
        return (status1 && status2 && mtypeB == typeB && mtypeM == typeM);
    }
    return false;
}

/// \brief Проверка корректности версии ВПО модуля.
bool Xml::ModuleParser::isCorrectModuleVersion(const QDomElement &moduleNode)
{
    auto versionNode = moduleNode.firstChildElement(tags::version);
    if (!versionNode.isNull())
    {
        auto version = versionNode.text();
        if (!version.isEmpty())
        {
            auto &sInfoBlock = Board::GetInstance().baseSerialInfo();
            return !(sInfoBlock.isOutdated(StdFunc::StrToVer(version)));
        }
    }
    return false;
}

/// \brief Коплексная проверка на корректность подключённого модуля.
/// \see isCorrectModuleVersion, isCorrectModuleType.
bool Xml::ModuleParser::isCorrectModule(const QDomElement &moduleNode, const quint16 &typeB, const quint16 &typeM)
{
    if (isCorrectModuleType(moduleNode, typeB, typeM))
    {
        if (isCorrectModuleVersion(moduleNode))
            return true;
        else
            qCritical() << "Устаревшая версия ВПО, обновите ВПО";
    }
    else
        qCritical() << "Invalid module type specified in XML configuration";
    return false;
}

/// \brief Функция для определения типа группы сигналов.
ModuleTypes::SignalType Xml::ModuleParser::parseSigType(const QDomNode &sigNode)
{
    auto typeNode = sigNode.firstChildElement(tags::type);
    if (!typeNode.isNull())
    {
        auto typeStr = typeNode.text();
        if (typeStr.contains("Float", Qt::CaseInsensitive))
            return ModuleTypes::SignalType::Float;
        else if (typeStr.contains("BitString", Qt::CaseInsensitive))
            return ModuleTypes::SignalType::BitString;
        else if (typeStr.contains("SinglePoint", Qt::CaseInsensitive))
            return ModuleTypes::SignalType::SinglePointWithTime;
    }
    return ModuleTypes::SignalType::Float;
}

/// \brief Функция для определения типа типа отображения мультивиджета.
ModuleTypes::ViewType Xml::ModuleParser::parseViewType(const QDomNode &mwidgetNode)
{
    auto viewString = mwidgetNode.toElement().attribute(tags::view, "float");
    if (viewString.contains("bitset", Qt::CaseInsensitive))
        return ModuleTypes::ViewType::Bitset;
    else
        return ModuleTypes::ViewType::Float;
}

/// \brief Функция для парсинга узла <signals>.
void Xml::ModuleParser::parseSignal(const QDomNode &sigNode)
{
    auto id = parseNumFromNode<quint32>(sigNode, tags::id);
    auto addr = parseNumFromNode<quint32>(sigNode, tags::start_addr);
    auto count = parseNumFromNode<quint16>(sigNode, tags::count);
    auto sigType = parseSigType(sigNode);
    emit signalDataSending(id, addr, count, sigType);
}

/// \brief Функция для парсинга узла <section-tabs>.
void Xml::ModuleParser::parseSTab(const QDomNode &sTabNode)
{
    auto id = parseNumFromNode<quint32>(sTabNode, tags::id);
    auto name = parseString(sTabNode, tags::name);
    emit tabDataSending(id, name);
}

/// \brief Функция для парсинга узла <sections>.
void Xml::ModuleParser::parseSection(const QDomNode &sectionNode)
{
    using namespace ModuleTypes;
    auto secHeader = sectionNode.toElement().attribute(tags::header, "");
    SGMap sgmap;
    callForEachChild(sectionNode, [&](const QDomNode &sgroupNode) {
        SGroup sgroup;
        auto sgroupElem = sgroupNode.toElement();
        auto sgroupHeader = sgroupElem.attribute(tags::header, "");
        auto sgroupTab = sgroupElem.attribute(tags::tab, "").toUInt();
        callForEachChild(sgroupNode, [&](const QDomNode &mwidgetNode) {
            auto mwidgetDesc = mwidgetNode.toElement().attribute(tags::desc, "");
            auto addr = parseNumFromNode<quint32>(mwidgetNode, tags::start_addr);
            auto count = parseNumFromNode<quint32>(mwidgetNode, tags::count);
            count = (count == 0) ? 1 : count;
            auto tooltip = parseString(mwidgetNode, tags::tooltip);
            auto view = parseViewType(mwidgetNode);
            auto itemList = parseStringArray(mwidgetNode);
            sgroup.name = sgroupHeader;
            sgroup.widgets.push_back({ mwidgetDesc, addr, count, tooltip, view, itemList });
        });
        sgmap.insert(sgroupTab, sgroup);
    });
    emit sectionDataSending(sgmap, secHeader);
}

/// \brief Функция для парсинга узла <alarms>.
void Xml::ModuleParser::parseAlarms(const QDomNode &alarmsNode)
{
    parseNode(alarmsNode, tags::crit, [this](const QDomNode &alarmNode) { //
        parseAlarm(alarmNode, Modules::AlarmType::Critical);
    });
    parseNode(alarmsNode, tags::warn, [this](const QDomNode &alarmNode) { //
        parseAlarm(alarmNode, Modules::AlarmType::Warning);
    });
    parseNode(alarmsNode, tags::info, [this](const QDomNode &alarmNode) { //
        parseAlarm(alarmNode, Modules::AlarmType::Info);
    });
}

/// \brief Функция для парсинга узлов <critical>, <warning> и <info> внутри <alarms>.
void Xml::ModuleParser::parseAlarm(const QDomNode &alarmNode, const Modules::AlarmType &aType)
{
    auto addr = parseNumFromNode<quint32>(alarmNode, tags::addr);
    auto desc = parseString(alarmNode, tags::string);
    auto hlValues = parseNumArray<quint32>(alarmNode, tags::highlights);
    emit alarmDataSending(aType, addr, desc, hlValues);
}

/// \brief Функция для парсинга узла <journals>.
void Xml::ModuleParser::parseJournals(const QDomNode &joursNode)
{
    parseNode(joursNode, tags::work, [this](const QDomNode &jourNode) { //
        parseJournal(jourNode, Modules::JournalType::Work);
    });
    parseNode(joursNode, tags::meas, [this](const QDomNode &jourNode) { //
        parseJournal(jourNode, Modules::JournalType::Meas);
    });
}

/// \brief Функция для парсинга узлов <work> и <meas> внутри <journals>.
void Xml::ModuleParser::parseJournal(const QDomNode &jourNode, const Modules::JournalType &jType)
{
    quint32 addr = 0;
    if (jType == Modules::JournalType::Work)
        addr = parseNumFromNode<quint32>(jourNode, tags::addr);
    auto tag = (jType == Modules::JournalType::Meas) ? tags::header : tags::desc;
    auto desc = parseString(jourNode, tag);
    emit jourDataSending(jType, addr, desc);
}

/// \brief Функция для парсинга конфигурации интерфейса, по которому подключен модуль.
void Xml::ModuleParser::parseInterface(const QDomNode &resNode)
{
    auto ifaceType = Board::GetInstance().interfaceType();
    QVariant ifaceConfig;
    if (ifaceType == Board::USB || ifaceType == Board::Emulator)
    {
        InterfaceInfo<ProtocomGroup> ifaceSettings;
        parseNode(resNode, tags::protocom, //
            [&](const QDomNode &protocomNode) { parseProtocom(protocomNode, ifaceSettings); });
        ifaceConfig.setValue(ifaceSettings);
    }
    else if (ifaceType == Board::RS485)
    {
        InterfaceInfo<ModbusGroup> ifaceSettings;
        parseNode(resNode, tags::modbus, //
            [&](const QDomNode &modbusNode) { parseModbus(modbusNode, ifaceSettings); });
        ifaceConfig.setValue(ifaceSettings);
    }
    else if (ifaceType == Board::Ethernet)
    {
        InterfaceInfo<Iec104Group> ifaceSettings;
        parseNode(resNode, tags::iec, //
            [&](const QDomNode &iecNode) { parseIec(iecNode, ifaceSettings); });
        ifaceConfig.setValue(ifaceSettings);
    }
    emit interfaceSettingsSending(ifaceConfig, ifaceType);
}

/// \brief Функция для парсинга узла <modbus>.
void Xml::ModuleParser::parseModbus(const QDomNode &modbusNode, InterfaceInfo<ModbusGroup> &settings)
{
    auto signalId = parseNumFromNode<quint32>(modbusNode, tags::sig_id);
    auto regType = parseNumFromNode<quint16>(modbusNode, tags::reg_type);
    auto type = parseString(modbusNode, tags::type);
    if (signalId != 0)
        settings.addGroup({ signalId, regType, type });
}

/// \brief Функция для парсинга узла <protocom>.
void Xml::ModuleParser::parseProtocom(const QDomNode &protocomNode, InterfaceInfo<ProtocomGroup> &settings)
{
    auto signalId = parseNumFromNode<quint32>(protocomNode, tags::sig_id);
    auto block = parseNumFromNode<quint16>(protocomNode, tags::block);
    if (signalId != 0)
        settings.addGroup({ signalId, block });
}

/// \brief Функция для парсинга узла <iec60870>.
void Xml::ModuleParser::parseIec(const QDomNode &iecNode, InterfaceInfo<Iec104Group> &settings)
{
    auto signalId = parseNumFromNode<quint32>(iecNode, tags::sig_id);
    auto sigType = parseNumFromNode<quint16>(iecNode, tags::sig_type);
    auto transType = parseNumFromNode<quint16>(iecNode, tags::trans_type);
    auto sigGroup = parseNumFromNode<quint16>(iecNode, tags::sig_group);
    if (signalId != 0)
        settings.addGroup({ signalId, sigType, transType, sigGroup });
}

/// \brief Функция для парсинга узла <config>.
void Xml::ModuleParser::parseConfig(const QDomNode &configNode)
{
    auto id = parseNumFromNode<quint16>(configNode, tags::id);
    auto defVal = parseString(configNode, tags::def_val);
    auto visibility = true;
    if (parseString(configNode, tags::visibility) == "false")
        visibility = false;
    auto count = parseNumFromNode<quint16>(configNode, tags::count);
    emit configDataSending(id, defVal, visibility, count);
}

/// \brief Функция для парсинга узла <resources>.
void Xml::ModuleParser::parseResources(const QDomElement &resNode)
{
    parseNode(resNode, tags::sigs, [this](const QDomNode &sigNode) { parseSignal(sigNode); });
    parseNode(resNode, tags::tabs, [this](const QDomNode &sTabNode) { parseSTab(sTabNode); });
    parseNode(resNode, tags::sections, [this](const QDomNode &sectionNode) { parseSection(sectionNode); });
    callIfNodeExist(resNode, tags::alarms, [this](const QDomNode &alarmsNode) { parseAlarms(alarmsNode); });
    callIfNodeExist(resNode, tags::journals, [this](const QDomNode &joursNode) { parseJournals(joursNode); });
    parseInterface(resNode);
    parseNode(resNode, tags::config, [this](const QDomNode &configNode) { parseConfig(configNode); });
}

/// \brief Функция для парсинга файла конфигурации модуля.
void Xml::ModuleParser::parse(const QDomNode &content, const quint16 &typeB, const quint16 &typeM)
{
    emit startNewConfig();
    isBase = (typeB == 0) ? false : true;
    auto moduleNode = content.firstChildElement(tags::module);
    if (!moduleNode.isNull())
    {
        if (isCorrectModule(moduleNode, typeB, typeM))
        {
            auto resNode = moduleNode.firstChildElement(tags::res);
            if (!resNode.isNull())
                parseResources(resNode);
        }
    }
}
