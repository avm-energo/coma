#include "xml/xmlparser/xmlmoduleparser.h"

#include <common/appconfig.h>
#include <device/current_device.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <gen/xml/xmlbase.h>
#include <gen/xml/xmlparse.h>
#include <xml/xmltags.h>

Xml::ModuleParser::ModuleParser(QObject *parent) : m_ifaceType(Interface::IfaceType::Unknown) { }

QString Xml::ModuleParser::getFileName(const quint16 typeB, const quint16 typeM)
{
    return QString("%1%2.xml").arg(typeB, 2, 16, QChar('0')).arg(typeM, 2, 16, QChar('0'));
}

bool Xml::ModuleParser::isCorrectDeviceType(const QDomElement &moduleNode, const Device::CurrentDevice *device)
{
    if (moduleNode.hasAttribute(tags::mtypeb) && moduleNode.hasAttribute(tags::mtypem))
    {
        auto mTypeB = moduleNode.attribute(tags::mtypeb, "");
        auto mTypeM = moduleNode.attribute(tags::mtypem, "");
        bool status1 = false, status2 = false;
        const u16 mtypeB = mTypeB.toUShort(&status1, 16);
        const u16 mtypeM = mTypeM.toUShort(&status2, 16);
        if (status1 && status2)
        {
            if (mtypeB == 0)
                return (mtypeM == device->getMezzType());
            else if (mtypeM == 0)
                return (mtypeB == device->getBaseType());
            else
                return ((mtypeB == device->getBaseType()) && (mtypeM == device->getMezzType()));
        }
    }
    return false;
}

bool Xml::ModuleParser::isCorrectFirmwareVersion(const QDomElement &moduleNode, const Device::CurrentDevice *device)
{
    auto versionNode = moduleNode.firstChildElement(tags::version);
    if (!versionNode.isNull())
    {
        auto version = versionNode.text();
        if (!version.isEmpty())
            return !(device->isOutdatedFirmware(StdFunc::StrToVer(version)));
    }
    return false;
}

bool Xml::ModuleParser::isCorrectDevice(const QDomElement &moduleNode, const Device::CurrentDevice *device)
{
    if (isCorrectDeviceType(moduleNode, device))
    {
        // Проверяем корректность версии только для базы
        if (moduleNode.attribute(tags::mtypeb, "").toUShort(nullptr, 16) != 0)
        {
            if (isCorrectFirmwareVersion(moduleNode, device))
                return true;
            else
                emit parseError("Устаревшая версия ВПО, обновите ВПО");
        }
        else
            return true;
    }
    else
        emit parseError("Типы плат устройства не соответствуют указанным в XML конфигурации");
    return false;
}

Xml::SignalType Xml::ModuleParser::parseSignalType(const QDomNode &signalNode)
{
    auto typeNode = signalNode.firstChildElement(tags::type);
    if (!typeNode.isNull())
    {
        auto typeStr = typeNode.text();
        if (typeStr.contains("Float", Qt::CaseInsensitive))
            return Xml::SignalType::Float;
        else if (typeStr.contains("BitString", Qt::CaseInsensitive))
            return Xml::SignalType::BitString;
        else if (typeStr.contains("SinglePoint", Qt::CaseInsensitive))
            return Xml::SignalType::SinglePointWithTime;
        else if (typeStr.contains("Command", Qt::CaseInsensitive))
            return Xml::SignalType::Command;
    }
    return Xml::SignalType::Float;
}

Xml::ViewType Xml::ModuleParser::parseViewType(const QString &viewString)
{
    if (viewString.contains("bitset", Qt::CaseInsensitive))
        return Xml::ViewType::Bitset;
    else if (viewString.contains("LineEdit", Qt::CaseInsensitive))
        return Xml::ViewType::LineEdit;
    else if (viewString.contains("Version", Qt::CaseInsensitive))
        return Xml::ViewType::Version;
    else if (viewString.contains("SinglePoint", Qt::CaseInsensitive))
        return Xml::ViewType::SinglePoint;
    else if (viewString.contains("command", Qt::CaseInsensitive))
        return Xml::ViewType::Command;
    else if (viewString.contains("commandvalue", Qt::CaseInsensitive))
        return Xml::ViewType::CommandWValue;
    else
        return Xml::ViewType::Float;
}

Xml::BinaryType Xml::ModuleParser::parseBinaryType(const QString &typeStr)
{
    if (typeStr == "uint32")
        return Xml::BinaryType::uint32;
    else if (typeStr == "time32")
        return Xml::BinaryType::time32;
    else if (typeStr == "time64")
        return Xml::BinaryType::time64;
    else if (typeStr == "string32")
        return Xml::BinaryType::string32;
    else if (typeStr == "version32")
        return Xml::BinaryType::version32;
    else
        return Xml::BinaryType::float32;
}

Xml::AlarmType Xml::ModuleParser::parseAlarmType(const QString &typeStr)
{
    if (typeStr.contains(tags::crit, Qt::CaseInsensitive))
        return Xml::AlarmType::Critical;
    else if (typeStr.contains(tags::warn, Qt::CaseInsensitive))
        return Xml::AlarmType::Warning;
    else
        return Xml::AlarmType::Info;
}

void Xml::ModuleParser::parseSignal(const QDomNode &sigNode)
{
    auto id = XmlParse::XmlParse::parseNumFromNode<u32>(sigNode, tags::id);
    auto addr = XmlParse::XmlParse::parseNumFromNode<u32>(sigNode, tags::start_addr);
    auto count = XmlParse::XmlParse::parseNumFromNode<u16>(sigNode, tags::count);
    auto sigType = parseSignalType(sigNode);
    emit signalDataSending(id, addr, count, sigType);
}

void Xml::ModuleParser::parseSTab(const QDomNode &sTabNode)
{
    bool isDebug;
    auto id = XmlParse::parseNumFromNode<u32>(sTabNode, tags::id);
    auto name = XmlParse::parseString(sTabNode, tags::name);
    auto stabDType = XmlParse::parseString(sTabNode, tags::dtype);
    isDebug = AppConfiguration::notDenied(stabDType);
    emit tabDataSending(id, { name, isDebug });
}

void Xml::ModuleParser::parseSection(const QDomNode &sectionNode)
{
    using namespace Xml;
    auto secHeader = sectionNode.toElement().attribute(tags::header, "");
    auto secOrder = sectionNode.toElement().attribute(tags::order, 0).toUInt();
    SGMap sgmap;
    XmlParse::callForEachChild(sectionNode,
        [&](const QDomNode &sgroupNode)
        {
            SGroup sgroup;
            auto sgroupElem = sgroupNode.toElement();
            auto sgroupHeader = sgroupElem.attribute(tags::header, "");
            auto sgroupOrder = sgroupElem.attribute(tags::order, "");
            auto sgroupTab = sgroupElem.attribute(tags::tab, "").toUInt();
            auto sgroupDType = sgroupElem.attribute(tags::dtype, "");
            XmlParse::callForEachChild(sgroupNode,
                [&](const QDomNode &mwidgetNode)
                {
                    auto mwidgetElem = mwidgetNode.toElement();
                    auto mwidgetDesc = mwidgetElem.attribute(tags::desc, "");
                    auto viewString = mwidgetElem.attribute(tags::view, "float");
                    auto addr = XmlParse::parseNumFromNode<u32>(mwidgetNode, tags::start_addr);
                    auto count = XmlParse::parseNumFromNode<u32>(mwidgetNode, tags::count);
                    count = (count == 0) ? 1 : count;
                    auto tooltip = XmlParse::parseString(mwidgetNode, tags::tooltip);
                    auto type = XmlParse::parseNumFromNode<u32>(mwidgetNode, tags::type);
                    auto decimals = XmlParse::parseNumFromNode<u32>(
                        mwidgetNode, tags::decimals, 3); // 3 decimals by default for float widgets
                    auto view = parseViewType(viewString);
                    auto itemList = XmlParse::parseArray(mwidgetNode, tags::str_array);
                    sgroup.widgets.push_back({ mwidgetDesc, addr, count, tooltip, view, decimals, itemList, type });
                });
            sgroup.name = sgroupHeader;
            sgroup.order = sgroupOrder.toInt();
            sgroup.notDenied = AppConfiguration::notDenied(sgroupDType);
            sgmap.insert(sgroupTab, sgroup);
        });
    emit sectionDataSending(sgmap, secOrder, secHeader);
}

void Xml::ModuleParser::parseAlarms(const QDomNode &alarmsNode)
{
    XmlParse::parseNode(alarmsNode, tags::state_all, [this](const QDomNode &alarmNode) { //
        parseAlarmStateAll(alarmNode);
    });
    XmlParse::parseNode(alarmsNode, tags::crit, [this](const QDomNode &alarmNode) { //
        parseAlarm(alarmNode, Xml::AlarmType::Critical);
    });
    XmlParse::parseNode(alarmsNode, tags::warn, [this](const QDomNode &alarmNode) { //
        parseAlarm(alarmNode, Xml::AlarmType::Warning);
    });
    XmlParse::parseNode(alarmsNode, tags::info, [this](const QDomNode &alarmNode) { //
        parseAlarm(alarmNode, Xml::AlarmType::Info);
    });
}

void Xml::ModuleParser::parseAlarmStateAll(const QDomNode &alarmStateAllNode)
{
    auto index = XmlParse::parseNumFromNode<u32>(alarmStateAllNode, tags::addr);
    auto desc = XmlParse::parseString(alarmStateAllNode, tags::string);
    auto type = parseAlarmType(XmlParse::parseString(alarmStateAllNode, tags::type));
    emit alarmStateAllDataSending(type, index, desc);
}

void Xml::ModuleParser::parseAlarm(const QDomNode &alarmNode, const AlarmType &type)
{
    auto addr = XmlParse::parseNumFromNode<u32>(alarmNode, tags::addr);
    auto desc = XmlParse::parseString(alarmNode, tags::string);
    auto order = XmlParse::parseNumFromNode<u32>(alarmNode, tags::order, U16MAX);
    order = (order == 0) ? U16MAX : order; // if an order is not set force to set it to the lowest priority
    auto hlValues = XmlParse::parseNumArray<u32>(alarmNode, tags::highlights);
    emit alarmDataSending(type, addr, desc, order, hlValues);
}

void Xml::ModuleParser::parseJournals(const QDomNode &joursNode)
{
    XmlParse::parseNode(joursNode, tags::work, [this](const QDomNode &jourNode) { parseWorkJournal(jourNode); });
    XmlParse::parseNode(joursNode, tags::meas, [this](const QDomNode &jourNode) { parseMeasJournal(jourNode); });
}

void Xml::ModuleParser::parseWorkJournal(const QDomNode &jourNode)
{
    auto id = XmlParse::parseNumFromNode<u32>(jourNode, tags::addr);
    auto desc = XmlParse::parseString(jourNode, tags::desc);
    emit workJourDataSending(id, desc);
}

void Xml::ModuleParser::parseMeasJournal(const QDomNode &jourNode)
{
    auto index = XmlParse::parseNumFromNode<u32>(jourNode, tags::index);
    auto header = XmlParse::parseString(jourNode, tags::header);
    auto strType = XmlParse::parseString(jourNode, tags::type);
    Xml::BinaryType type = parseBinaryType(strType);
    auto visibility = true;
    if (XmlParse::parseString(jourNode, tags::visibility) == "false")
        visibility = false;
    emit measJourDataSending(index, header, type, visibility);
}

void Xml::ModuleParser::parseInterface(const QDomNode &resNode)
{
    switch (m_ifaceType)
    {
    case Interface::IfaceType::USB:
    case Interface::IfaceType::Emulator:
        XmlParse::parseNode(
            resNode, tags::protocom, [&](const QDomNode &protocolNode) { parseProtocom(protocolNode); });
        break;
    case Interface::IfaceType::RS485:
        XmlParse::parseNode(resNode, tags::modbus, [&](const QDomNode &protocolNode) { parseModbus(protocolNode); });
        break;
    case Interface::IfaceType::Ethernet:
        XmlParse::parseNode(resNode, tags::iec, [&](const QDomNode &protocolNode) { parseIec(protocolNode); });
        break;
    default:
        Q_ASSERT(false);
        qDebug() << "Undefined interface type";
        return;
    }
}

void Xml::ModuleParser::parseModbus(const QDomNode &modbusNode)
{
    auto signalId = XmlParse::parseNumFromNode<u32>(modbusNode, tags::sig_id);
    auto regType = XmlParse::parseNumFromNode<u16>(modbusNode, tags::reg_type);
    if (signalId != 0)
    {
        Protocol::AbstractGroup str { Interface::IfaceType::RS485, signalId, regType, quint16() };
        emit protocolGroupSending(str);
    }
}

void Xml::ModuleParser::parseProtocom(const QDomNode &protocomNode)
{
    auto signalId = XmlParse::parseNumFromNode<u32>(protocomNode, tags::sig_id);
    auto block = XmlParse::parseNumFromNode<u16>(protocomNode, tags::block);
    if (signalId != 0)
    {
        Protocol::AbstractGroup str { Interface::IfaceType::USB, signalId, block, quint16() };
        emit protocolGroupSending(str);
    }
}

void Xml::ModuleParser::parseIec(const QDomNode &iecNode)
{
    auto signalId = XmlParse::parseNumFromNode<u32>(iecNode, tags::sig_id);
    auto transType = XmlParse::parseNumFromNode<u16>(iecNode, tags::trans_type);
    auto sigGroup = XmlParse::parseNumFromNode<u16>(iecNode, tags::sig_group);
    if (signalId != 0)
    {
        Protocol::AbstractGroup str { Interface::IfaceType::Ethernet, signalId, transType, sigGroup };
        emit protocolGroupSending(str);
    }
}

void Xml::ModuleParser::parseConfig(const QDomNode &configNode)
{
    auto id = XmlParse::parseNumFromNode<u16>(configNode, tags::id);
    auto defVal = XmlParse::parseString(configNode, tags::def_val);
    auto visibility = true;
    if (XmlParse::parseString(configNode, tags::visibility) == "false")
        visibility = false;
    auto count = XmlParse::parseNumFromNode<u16>(configNode, tags::count);
    u16 order = XmlParse::parseNumFromNode<u16>(configNode, tags::order, U16MAX);
    emit configDataSending(id, defVal, visibility, count, order);
}

void Xml::ModuleParser::parseHiddenTab(const QDomNode &hiddenTabNode)
{
    auto hiddenTabElem = hiddenTabNode.toElement();
    auto tabTitle = hiddenTabElem.attribute(tags::desc);
    auto tabPrefix = hiddenTabElem.attribute(tags::prefix);
    auto tabFlag = (hiddenTabElem.attribute(tags::flag)).toUShort();
    std::vector<Xml::HiddenWidget> widgets;
    XmlParse::callForEachChild(hiddenTabNode,
        [this, &widgets](const QDomNode &hiddenWidgetNode)
        {
            auto hiddenWidgetElem = hiddenWidgetNode.toElement();
            auto viewStr = hiddenWidgetElem.attribute(tags::view, "LineEdit");
            auto title = hiddenWidgetElem.attribute(tags::title);
            auto view = parseViewType(viewStr);
            auto name = XmlParse::parseString(hiddenWidgetNode, tags::name);
            auto typeStr = XmlParse::parseString(hiddenWidgetNode, tags::type);
            auto type = parseBinaryType(typeStr);
            auto address = XmlParse::parseNumFromNode<u32>(hiddenWidgetNode, tags::addr);
            auto index = XmlParse::parseNumFromNode<u16>(hiddenWidgetNode, tags::index);
            auto visibility = true;
            if (XmlParse::parseString(hiddenWidgetNode, tags::visibility) == "false")
                visibility = false;
            widgets.push_back(Xml::HiddenWidget { name, title, address, index, type, view, visibility });
        });
    emit hiddenTabDataSending(Xml::HiddenTab { tabTitle, tabPrefix, tabFlag, widgets });
}

void Xml::ModuleParser::parseBsiExtItem(const QDomNode &bsiExtItemNode)
{
    auto address = XmlParse::parseNumFromNode<u32>(bsiExtItemNode, tags::addr);
    auto typeStr = XmlParse::parseString(bsiExtItemNode, tags::type);
    auto desc = XmlParse::parseString(bsiExtItemNode, tags::desc);
    auto type = parseBinaryType(typeStr);
    auto visibility = true;
    if (XmlParse::parseString(bsiExtItemNode, tags::visibility) == "false")
        visibility = false;
    emit bsiExtItemDataSending(address, type, visibility, desc);
}

void Xml::ModuleParser::parseOverlayConfigTab(const QDomNode &tabNode)
{
    auto id = XmlParse::parseNumFromNode<u32>(tabNode, tags::id);
    auto name = XmlParse::parseString(tabNode, tags::name);
    emit configTabDataSending(id, name);
}

void Xml::ModuleParser::parseOverlayRecord(const QDomNode &recordNode)
{
    auto name = recordNode.nodeName();
    auto id = quint32(0);
    auto idNode = recordNode.firstChildElement(tags::id);
    if (!idNode.isNull())
    {
        id = XmlParse::parseNum<u32>(idNode);
        if (XmlParse::isNodeExist(recordNode, tags::name))
            emit nameDataSending(id, XmlParse::parseString(recordNode, tags::name));
        if (XmlParse::isNodeExist(recordNode, tags::dtype))
            emit dtypeDataSending(id, AppConfiguration::notDenied(XmlParse::parseString(recordNode, tags::dtype)));
        if (XmlParse::isNodeExist(recordNode, tags::type))
            emit typeDataSending(id, parseType(XmlParse::elementByTag(recordNode, tags::type)));
        if (XmlParse::isNodeExist(recordNode, tags::widget))
            emit widgetDataSending(id, parseWidget(XmlParse::elementByTag(recordNode, tags::widget)));
    }
}

void Xml::ModuleParser::parseDetector(const QDomNode &node)
{
    const auto tag = node.toElement().tagName();
    if (tag == tags::sigs)
        XmlParse::callForEachChild(node, [this](const QDomNode &sigNode) { parseSignal(sigNode); });
    else if (tag == tags::tabs)
        XmlParse::callForEachChild(node, [this](const QDomNode &sTabNode) { parseSTab(sTabNode); });
    else if (tag == tags::sections)
        XmlParse::callForEachChild(node, [this](const QDomNode &sectionNode) { parseSection(sectionNode); });
    else if (tag == tags::hidden)
        XmlParse::callForEachChild(node, [this](const QDomNode &hiddenTabNode) { parseHiddenTab(hiddenTabNode); });
    else if (tag == tags::bsi_ext)
        XmlParse::callForEachChild(node, [this](const QDomNode &bsiExtItemNode) { parseBsiExtItem(bsiExtItemNode); });
    else if (tag == tags::alarms)
        parseAlarms(node);
    else if (tag == tags::journals)
        parseJournals(node);
    else if (tag == tags::config)
    {
        auto desc = node.toElement().attribute(tags::desc);
        emit configNameSending(desc);
        XmlParse::callForEachChild(node, [this](const QDomNode &configNode) { parseConfig(configNode); });
    }
    else
    {
        if (tag != tags::iec && tag != tags::modbus && tag != tags::protocom)
            qWarning() << "Undefined XML tag: " << tag;
    }
}

void Xml::ModuleParser::parseResources(const QDomElement &resourcesNode, const QStringList &nodes)
{
    if (!resourcesNode.isNull())
    {
        auto parseAction = [this](const QDomNode &node) { parseDetector(node); };
        // Парсим весь файл, если не указаны требуемые узлы
        if (nodes.empty())
        {
            emit startNewConfig();
            XmlParse::callForEachChild(resourcesNode, parseAction);
            // Настройки интерфейсов всегда парсим отдельно после парсинга сигналов
            parseInterface(resourcesNode);
        }
        else
        {
            for (auto &node : nodes)
                XmlParse::callIfNodeExist(resourcesNode, node, parseAction);
        }
    }
    else
        emit parseError("В файле не найден узел <resources>");
}

void Xml::ModuleParser::parseOverlay(const QDomElement &overlayNode)
{
    XmlParse::parseNode(
        overlayNode, tags::conf_tabs, [this](const QDomNode &tabNode) { parseOverlayConfigTab(tabNode); });
    XmlParse::parseNode(
        overlayNode, tags::records, [this](const QDomNode &recordNode) { parseOverlayRecord(recordNode); });
}

void Xml::ModuleParser::parseDocument(const QString &filename, const QStringList &nodes)
{
    if (Files::isFileExist(filename))
    {
        auto moduleNode = XmlBase::getXMLFirstElementFromFile(filename, tags::module);
        if (!moduleNode.isNull())
        {
            auto featuresNode = moduleNode.firstChildElement(tags::features);
            if (!featuresNode.isNull())
                XmlParse::parseNode(
                    featuresNode, tags::features, [&](const QDomNode &featuresNode) { parseFeatures(featuresNode); });
            auto resources = moduleNode.firstChildElement(tags::res);
            parseResources(resources, nodes);
            auto overlay = moduleNode.firstChildElement(tags::overlay);
            parseOverlay(overlay);
        }
        else
            emit parseError(QString("В файле %1 не найден узел <module>").arg(filename));
    }
    else
        emit parseError(QString("Файл %1 не найден").arg(filename));
}

void Xml::ModuleParser::parseDocument(const QStringList &filenames, const Device::CurrentDevice *device)
{
    for (const auto &filename : std::as_const(filenames))
    {
        if (Files::isFileExist(filename))
        {
            /*            auto document = getFileContent(filename);
                        auto moduleNode = document.firstChildElement(tags::module); */
            auto moduleNode = XmlBase::getXMLFirstElementFromFile(filename, tags::module);

            if (!moduleNode.isNull())
            {
                if (isCorrectDevice(moduleNode, device))
                {
                    auto featuresNode = moduleNode.firstChildElement(tags::features);
                    if (!featuresNode.isNull())
                        XmlParse::callForEachChild(
                            featuresNode, [&](const QDomNode &featuresNode) { parseFeatures(featuresNode); });
                    auto resources = moduleNode.firstChildElement(tags::res);
                    parseResources(resources);
                    auto overlay = moduleNode.firstChildElement(tags::overlay);
                    parseOverlay(overlay);
                }
                else
                    break;
            }
            else
                emit parseError(QString("В файле %1 не найден узел <module>").arg(filename));
        }
        else
            emit parseError(QString("Файл %1 не найден").arg(filename));
    }
}

void Xml::ModuleParser::parseFeatures(const QDomNode &featuresNode)
{
    auto featuresNodeElement = featuresNode.toElement();
    if (!featuresNodeElement.isNull())
    {
        const QString key = featuresNodeElement.tagName();
        const QString value = featuresNodeElement.text();
        emit newFeatureParsedSignal(key, value);
    }
}

void Xml::ModuleParser::parse(const u16 typeB, const u16 typeM, const QStringList &nodes)
{
    auto xmlFilename = getFileName(typeB, typeM);
    if (Files::isFileExist(xmlFilename))
        parseDocument(xmlFilename, nodes);
    else
    {
        auto baseXmlFilename = getFileName(typeB, 0);
        auto mezzXmlFilename = getFileName(0, typeM);
        parseDocument(baseXmlFilename, nodes);
        parseDocument(mezzXmlFilename, nodes);
    }
}

void Xml::ModuleParser::parse(Device::CurrentDevice *device)
{
    if (device)
    {
        m_ifaceType = device->async()->getInterfaceType();
        const auto typeB = device->getBaseType();
        const auto typeM = device->getMezzType();
        auto xmlFilename = getFileName(typeB, typeM);
        if (Files::isFileExist(xmlFilename))
            parseDocument({ xmlFilename }, device);
        else
        {
            auto baseXmlFilename = getFileName(typeB, 0);
            auto mezzXmlFilename = getFileName(0, typeM);
            parseDocument({ baseXmlFilename, mezzXmlFilename }, device);
        }
    }
    else
        emit parseError("Получен нулевой указатель на устройство");
}
