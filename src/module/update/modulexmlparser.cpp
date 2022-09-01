#include "modulexmlparser.h"

#include "../board.h"
#include "../gen/stdfunc.h"
#include "configstorage.h"

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

void ModuleXmlParser::parseSignals(const QDomNode &sigNode)
{
    auto id = parseNumFromNode<quint32>(sigNode, tags::id);
    auto addr = parseNumFromNode<quint64>(sigNode, tags::addr);
    auto count = parseNumFromNode<quint16>(sigNode, tags::count);
    emit signalDataSending(id, addr, count);
}

void ModuleXmlParser::parseSTabs(const QDomNode &sTabsNode)
{
    auto id = parseNumFromNode<quint32>(sTabsNode, tags::id);
    auto name = parseSting(sTabsNode, tags::name);
    emit tabDataSending(id, name);
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
    parseNode(resNode, tags::sigs, [this](const QDomNode &node) { parseSignals(node); });
    parseNode(resNode, tags::stabs, [this](const QDomNode &node) { parseSTabs(node); });

    // parseNode(resNode, tags::modbus, [this](const QDomNode &node) { parseModbus(node); });
    // parseNode(resNode, tags::protocom, [this](const QDomNode &node) { parseProtocom(node); });
    // parseNode(resNode, tags::iec, [this](const QDomNode &node) { parseIec(node); });
    // parseNode(resNode, tags::config, [this](const QDomNode &node) { parseConfig(node); });
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
