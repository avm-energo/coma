#include "modulexmlparser.h"

#include "../../gen/stdfunc.h"
#include "../../module/board.h"

ModuleXmlParser::ModuleXmlParser(QObject *parent) : BaseXmlParser(parent), isBase(true)
{
}

bool ModuleXmlParser::isCorrectModuleType(const QDomElement &moduleNode, const quint16 &typeB, const quint16 &typeM)
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

bool ModuleXmlParser::isCorrectModuleVersion(const QDomElement &moduleNode)
{
    auto versionNode = moduleNode.firstChildElement(tags::version);
    if (!versionNode.isNull())
    {
        auto version = versionNode.text();
        if (!version.isEmpty())
        {
            auto &sInfoBlock = Board::GetInstance().baseSerialInfo();
            return sInfoBlock.isOutdated(StdFunc::StrToVer(version));
        }
    }
    return false;
}

bool ModuleXmlParser::isCorrectModule(const QDomElement &moduleNode, const quint16 &typeB, const quint16 &typeM)
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

ModuleTypes::SignalType ModuleXmlParser::parseSigType(const QDomNode &sigNode)
{
    auto typeNode = sigNode.firstChildElement(tags::type);
    if (typeNode.isNull())
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

void ModuleXmlParser::parseSignal(const QDomNode &sigNode)
{
    auto id = parseNumFromNode<quint32>(sigNode, tags::id);
    auto addr = parseNumFromNode<quint32>(sigNode, tags::start_addr);
    auto count = parseNumFromNode<quint16>(sigNode, tags::count);
    auto sigType = parseSigType(sigNode);
    emit signalDataSending(id, addr, count, sigType);
}

void ModuleXmlParser::parseSTab(const QDomNode &sTabNode)
{
    auto id = parseNumFromNode<quint32>(sTabNode, tags::id);
    auto name = parseString(sTabNode, tags::name);
    emit tabDataSending(id, name);
}

void ModuleXmlParser::parseSection(const QDomNode &sectionNode)
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
            auto itemList = parseStringArray(mwidgetNode);
            sgroup.name = sgroupHeader;
            sgroup.widgets.push_back({ mwidgetDesc, addr, count, tooltip, itemList });
        });
        sgmap.insert(sgroupTab, sgroup);
    });
    emit sectionDataSending(sgmap, secHeader);
}

void ModuleXmlParser::parseAlarms(const QDomNode &alarmsNode)
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

void ModuleXmlParser::parseAlarm(const QDomNode &alarmNode, const Modules::AlarmType &aType)
{
    auto addr = parseNumFromNode<quint32>(alarmNode, tags::addr);
    auto desc = parseString(alarmNode, tags::string);
    emit alarmDataSending(isBase, aType, addr, desc);
}

void ModuleXmlParser::parseJournals(const QDomNode &joursNode)
{
    parseNode(joursNode, tags::work, [this](const QDomNode &jourNode) { //
        parseJournal(jourNode, Modules::JournalType::Work);
    });
    parseNode(joursNode, tags::meas, [this](const QDomNode &jourNode) { //
        parseJournal(jourNode, Modules::JournalType::Meas);
    });
}

void ModuleXmlParser::parseJournal(const QDomNode &jourNode, const Modules::JournalType &jType)
{
    quint32 addr = 0;
    if (jType == Modules::JournalType::Work)
        addr = parseNumFromNode<quint32>(jourNode, tags::addr);
    auto tag = (jType == Modules::JournalType::Meas) ? tags::header : tags::desc;
    auto desc = parseString(jourNode, tag);
    emit jourDataSending(jType, addr, desc);
}

void ModuleXmlParser::parseInterface(const QDomNode &root)
{
    auto ifaceType = Board::GetInstance().interfaceType();
    if (ifaceType == Board::USB || ifaceType == Board::Emulator)
    {
        ;
    }
    else if (ifaceType == Board::RS485)
    {
        ;
    }
    else if (ifaceType == Board::Ethernet)
    {
        ;
    }
}

void ModuleXmlParser::parseModbus(const QDomNode &modbusNode)
{
    auto signalId = parseNumFromNode<quint32>(modbusNode, tags::sig_id);
    auto regType = parseNumFromNode<quint16>(modbusNode, tags::reg_type);
    emit modbusDataSending(signalId, regType);
}

void ModuleXmlParser::parseProtocom(const QDomNode &protocomNode)
{
    auto signalId = parseNumFromNode<quint32>(protocomNode, tags::sig_id);
    auto block = parseNumFromNode<quint32>(protocomNode, tags::block);
    emit protocomDataSending(signalId, block);
}

void ModuleXmlParser::parseIec(const QDomNode &iecNode)
{
    auto sigId = parseNumFromNode<quint32>(iecNode, tags::sig_id);
    auto transType = parseNumFromNode<quint16>(iecNode, tags::trans_type);
    auto sigGroup = parseNumFromNode<quint16>(iecNode, tags::sig_group);
    emit iecDataSending(sigId, transType, sigGroup);
}

void ModuleXmlParser::parseConfig(const QDomNode &configNode)
{
    quint32 count = 0;
    auto id = parseNumFromNode<quint32>(configNode, tags::id);
    auto defVal = parseString(configNode, tags::def_val);
    auto visibility = true;
    if (parseString(configNode, tags::visibility) == "false")
        visibility = false;
    auto widgetNode = configNode.firstChildElement(tags::widget);
    if (!widgetNode.isNull())
        count = parseNumFromNode<quint32>(widgetNode, tags::count);
    emit configDataSending(id, defVal, visibility, count);
}

void ModuleXmlParser::parseResources(const QDomElement &resNode)
{
    parseNode(resNode, tags::sigs, [this](const QDomNode &sigNode) { parseSignal(sigNode); });
    parseNode(resNode, tags::tabs, [this](const QDomNode &sTabNode) { parseSTab(sTabNode); });
    parseNode(resNode, tags::sections, [this](const QDomNode &sectionNode) { parseSection(sectionNode); });
    callIfNodeExist(resNode, tags::alarms, [this](const QDomNode &alarmsNode) { parseAlarms(alarmsNode); });
    callIfNodeExist(resNode, tags::journals, [this](const QDomNode &joursNode) { parseJournals(joursNode); });
    parseNode(resNode, tags::modbus, [this](const QDomNode &modbusNode) { parseModbus(modbusNode); });
    parseNode(resNode, tags::protocom, [this](const QDomNode &protocomNode) { parseProtocom(protocomNode); });
    parseNode(resNode, tags::iec, [this](const QDomNode &iecNode) { parseIec(iecNode); });
    parseNode(resNode, tags::config, [this](const QDomNode &configNode) { parseConfig(configNode); });
}

void ModuleXmlParser::parse(const QDomNode &content, const quint16 &typeB, const quint16 &typeM)
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
        else
            return;
    }
}
