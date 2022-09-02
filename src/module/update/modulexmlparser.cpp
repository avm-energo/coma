#include "modulexmlparser.h"

#include "../board.h"
#include "../gen/stdfunc.h"

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

void ModuleXmlParser::parseSignal(const QDomNode &sigNode)
{
    auto id = parseNumFromNode<quint32>(sigNode, tags::id);
    auto addr = parseNumFromNode<quint64>(sigNode, tags::addr);
    auto count = parseNumFromNode<quint16>(sigNode, tags::count);
    emit signalDataSending(id, addr, count);
}

void ModuleXmlParser::parseSTab(const QDomNode &sTabNode)
{
    auto id = parseNumFromNode<quint32>(sTabNode, tags::id);
    auto name = parseString(sTabNode, tags::name);
    emit tabDataSending(id, name);
}

void ModuleXmlParser::parseSection(const QDomNode &sectionNode)
{
    auto secHeader = sectionNode.toElement().attribute(tags::header, "");
    callForEachChild(sectionNode, [this, &secHeader](const QDomNode &sgroupNode) {
        auto sgroupElem = sgroupNode.toElement();
        auto sgroupHeader = sgroupElem.attribute(tags::header, "");
        auto sgroupTab = sgroupElem.attribute(tags::tab, "");
        callForEachChild(sgroupNode, [&](const QDomNode &mwidgetNode) {
            auto mwidgetDesc = mwidgetNode.toElement().attribute(tags::desc, "");
            auto addr = parseNumFromNode<quint32>(mwidgetNode, tags::addr);
            auto count = parseNumFromNode<quint32>(mwidgetNode, tags::count);
            count = (count == 0) ? 1 : count;
            auto tooltip = parseString(mwidgetNode, tags::tooltip);
            auto itemList = parseStringArray(mwidgetNode);
            ModuleTypes::MWidget mwidget { mwidgetDesc, addr, count, tooltip, itemList };
            emit sectionDataSending(mwidget, sgroupTab, sgroupHeader, secHeader);
        });
    });
}

void ModuleXmlParser::parseAlarms(const QDomNode &alarmsNode)
{
    parseNode(alarmsNode, tags::crit, [this](const QDomNode &alarmsNode) {});
}

void ModuleXmlParser::parseAlarm(const QDomNode &alarmNode)
{
    ;
}

void ModuleXmlParser::parseModbus(const QDomNode &modbusNode)
{
    auto signalId = parseNumFromNode<quint32>(modbusNode, tags::sig_id);
    auto regType = parseNumFromNode<quint16>(modbusNode, tags::reg_type);
    auto typeStr = parseString(modbusNode, tags::type);
    auto type = ctti::id_from_name(typeStr.toStdString());
    emit modbusDataSending(signalId, regType, type);
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
    auto sigType = parseNumFromNode<quint16>(iecNode, tags::sig_type);
    auto transType = parseNumFromNode<quint16>(iecNode, tags::trans_type);
    auto sigGroup = parseNumFromNode<quint16>(iecNode, tags::sig_group);
    emit iecDataSending(sigId, sigType, transType, sigGroup);
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
    /*
    auto sectionsNode = resNode.firstChildElement(tags::sections);
    if (!sectionsNode.isNull())
    {
        ;
    }
    auto alarmsNode = resNode.firstChildElement(tags::alarms);
    if (!alarmsNode.isNull())
    {
        ;
    }
    auto journalsNode = resNode.firstChildElement(tags::jours);
    if (!journalsNode.isNull())
    {
        ;
    }
    */
    parseNode(resNode, tags::sigs, [this](const QDomNode &sigNode) { parseSignal(sigNode); });
    parseNode(resNode, tags::stabs, [this](const QDomNode &sTabNode) { parseSTab(sTabNode); });
    parseNode(resNode, tags::sections, [this](const QDomNode &sectionNode) { parseSection(sectionNode); });

    parseNode(resNode, tags::modbus, [this](const QDomNode &modbusNode) { parseModbus(modbusNode); });
    parseNode(resNode, tags::protocom, [this](const QDomNode &protocomNode) { parseProtocom(protocomNode); });
    parseNode(resNode, tags::iec, [this](const QDomNode &iecNode) { parseIec(iecNode); });
    parseNode(resNode, tags::config, [this](const QDomNode &configNode) { parseConfig(configNode); });
}

void ModuleXmlParser::parse(const QDomNode &content, const quint16 &typeB, const quint16 &typeM)
{
    isBase = (typeB == 0) ? false : true;
    auto moduleNode = content.firstChildElement(tags::module);
    if (!moduleNode.isNull())
    {
        if (isCorrectModule(moduleNode, typeB, typeM))
        {
            auto resNode = moduleNode.firstChildElement(tags::resources);
            if (!resNode.isNull())
                parseResources(resNode);
        }
        else
            return;
    }
}
