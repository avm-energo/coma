#include "xmlmoduleparser.h"

#include <device/current_device.h>
#include <gen/stdfunc.h>

Xml::ModuleParser::ModuleParser(QObject *parent) : BaseParser(parent), m_ifaceType(Interface::IfaceType::Unknown)
{
}

QString Xml::ModuleParser::getFileName(const quint16 typeB, const quint16 typeM)
{
    auto typeBStr = QString::number(typeB, 16);
    typeBStr = (typeBStr.length() < 2) ? "0" + typeBStr : typeBStr;
    auto typeMStr = QString::number(typeM, 16);
    typeMStr = (typeMStr.length() < 2) ? "0" + typeMStr : typeMStr;
    return typeBStr + typeMStr + ".xml";
}

bool Xml::ModuleParser::isCorrectDeviceType(const QDomElement &moduleNode, Device::CurrentDevice *device)
{
    if (moduleNode.hasAttribute(tags::mtypeb) && moduleNode.hasAttribute(tags::mtypem))
    {
        auto mTypeB = moduleNode.attribute(tags::mtypeb, "");
        auto mTypeM = moduleNode.attribute(tags::mtypem, "");
        bool status1 = false, status2 = false;
        const u16 mtypeB = mTypeB.toUShort(&status1, 16);
        const u16 mtypeM = mTypeM.toUShort(&status2, 16);
        return (status1 && status2 && mtypeB == device->getBaseType() && mtypeM == device->getMezzType());
    }
    return false;
}

bool Xml::ModuleParser::isCorrectFirmwareVersion(const QDomElement &moduleNode, Device::CurrentDevice *device)
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

bool Xml::ModuleParser::isCorrectDevice(const QDomElement &moduleNode, Device::CurrentDevice *device)
{
    if (isCorrectDeviceType(moduleNode, device))
    {
        // Проверяем корректность версии только для базы
        auto isBase = (device == 0) ? false : true;
        if (isBase)
        {
            if (isCorrectFirmwareVersion(moduleNode, device))
                return true;
            else
                qCritical() << "Устаревшая версия ВПО, обновите ВПО";
        }
        else
            return true;
    }
    else
        qCritical() << "Типы плат устройства не соответствуют указанным в XML конфигурации";
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
    else
        return Xml::BinaryType::float32;
}

void Xml::ModuleParser::parseSignal(const QDomNode &sigNode)
{
    auto id = parseNumFromNode<u32>(sigNode, tags::id);
    auto addr = parseNumFromNode<u32>(sigNode, tags::start_addr);
    auto count = parseNumFromNode<u16>(sigNode, tags::count);
    auto sigType = parseSignalType(sigNode);
    emit signalDataSending(id, addr, count, sigType);
}

void Xml::ModuleParser::parseSTab(const QDomNode &sTabNode)
{
    auto id = parseNumFromNode<u32>(sTabNode, tags::id);
    auto name = parseString(sTabNode, tags::name);
    emit tabDataSending(id, name);
}

void Xml::ModuleParser::parseSection(const QDomNode &sectionNode)
{
    using namespace Xml;
    auto secHeader = sectionNode.toElement().attribute(tags::header, "");
    SGMap sgmap;
    callForEachChild(sectionNode, [&](const QDomNode &sgroupNode) {
        SGroup sgroup;
        auto sgroupElem = sgroupNode.toElement();
        auto sgroupHeader = sgroupElem.attribute(tags::header, "");
        auto sgroupTab = sgroupElem.attribute(tags::tab, "").toUInt();
        callForEachChild(sgroupNode, [&](const QDomNode &mwidgetNode) {
            auto mwidgetElem = mwidgetNode.toElement();
            auto mwidgetDesc = mwidgetElem.attribute(tags::desc, "");
            auto viewString = mwidgetElem.attribute(tags::view, "float");
            auto addr = parseNumFromNode<u32>(mwidgetNode, tags::start_addr);
            auto count = parseNumFromNode<u32>(mwidgetNode, tags::count);
            count = (count == 0) ? 1 : count;
            auto tooltip = parseString(mwidgetNode, tags::tooltip);
            auto view = parseViewType(viewString);
            auto itemList = parseStringArray(mwidgetNode);
            sgroup.name = sgroupHeader;
            sgroup.widgets.push_back({ mwidgetDesc, addr, count, tooltip, view, itemList });
        });
        sgmap.insert(sgroupTab, sgroup);
    });
    emit sectionDataSending(sgmap, secHeader);
}

void Xml::ModuleParser::parseAlarms(const QDomNode &alarmsNode)
{
    parseNode(alarmsNode, tags::crit, [this](const QDomNode &alarmNode) { //
        parseAlarm(alarmNode, Xml::AlarmType::Critical);
    });
    parseNode(alarmsNode, tags::warn, [this](const QDomNode &alarmNode) { //
        parseAlarm(alarmNode, Xml::AlarmType::Warning);
    });
    parseNode(alarmsNode, tags::info, [this](const QDomNode &alarmNode) { //
        parseAlarm(alarmNode, Xml::AlarmType::Info);
    });
}

void Xml::ModuleParser::parseAlarm(const QDomNode &alarmNode, const AlarmType &type)
{
    auto addr = parseNumFromNode<u32>(alarmNode, tags::addr);
    auto desc = parseString(alarmNode, tags::string);
    auto hlValues = parseNumArray<u32>(alarmNode, tags::highlights);
    emit alarmDataSending(type, addr, desc, hlValues);
}

void Xml::ModuleParser::parseJournals(const QDomNode &joursNode)
{
    parseNode(joursNode, tags::work, [this](const QDomNode &jourNode) { parseWorkJournal(jourNode); });
    parseNode(joursNode, tags::meas, [this](const QDomNode &jourNode) { parseMeasJournal(jourNode); });
}

void Xml::ModuleParser::parseWorkJournal(const QDomNode &jourNode)
{
    auto id = parseNumFromNode<u32>(jourNode, tags::addr);
    auto desc = parseString(jourNode, tags::desc);
    emit workJourDataSending(id, desc);
}

void Xml::ModuleParser::parseMeasJournal(const QDomNode &jourNode)
{
    auto index = parseNumFromNode<u32>(jourNode, tags::index);
    auto header = parseString(jourNode, tags::header);
    auto strType = parseString(jourNode, tags::type);
    Xml::BinaryType type = parseBinaryType(strType);
    auto visibility = true;
    if (parseString(jourNode, tags::visibility) == "false")
        visibility = false;
    emit measJourDataSending(index, header, type, visibility);
}

void Xml::ModuleParser::parseInterface(const QDomNode &resNode)
{
    // auto ifaceType = Interface::ActiveConnection::async()->getInterfaceType();
    switch (m_ifaceType)
    {
    case Interface::IfaceType::USB:
    case Interface::IfaceType::Emulator:
        parseNode(resNode, tags::protocom, [&](const QDomNode &protocolNode) { parseProtocom(protocolNode); });
        break;
    case Interface::IfaceType::RS485:
        parseNode(resNode, tags::modbus, [&](const QDomNode &protocolNode) { parseModbus(protocolNode); });
        break;
    case Interface::IfaceType::Ethernet:
        parseNode(resNode, tags::iec, [&](const QDomNode &protocolNode) { parseIec(protocolNode); });
        break;
    default:
        Q_ASSERT(false);
        qCritical() << "Undefined interface type";
        return;
    }
}

void Xml::ModuleParser::parseModbus(const QDomNode &modbusNode)
{
    auto signalId = parseNumFromNode<u32>(modbusNode, tags::sig_id);
    auto regType = parseNumFromNode<u16>(modbusNode, tags::reg_type);
    if (signalId != 0)
    {
        Protocol::AbstractGroup str { Interface::IfaceType::RS485, signalId, regType, quint16() };
        emit protocolGroupSending(str);
    }
}

void Xml::ModuleParser::parseProtocom(const QDomNode &protocomNode)
{
    auto signalId = parseNumFromNode<u32>(protocomNode, tags::sig_id);
    auto block = parseNumFromNode<u16>(protocomNode, tags::block);
    if (signalId != 0)
    {
        Protocol::AbstractGroup str { Interface::IfaceType::USB, signalId, block, quint16() };
        emit protocolGroupSending(str);
    }
}

void Xml::ModuleParser::parseIec(const QDomNode &iecNode)
{
    auto signalId = parseNumFromNode<u32>(iecNode, tags::sig_id);
    auto transType = parseNumFromNode<u16>(iecNode, tags::trans_type);
    auto sigGroup = parseNumFromNode<u16>(iecNode, tags::sig_group);
    if (signalId != 0)
    {
        Protocol::AbstractGroup str { Interface::IfaceType::Ethernet, signalId, transType, sigGroup };
        emit protocolGroupSending(str);
    }
}

void Xml::ModuleParser::parseConfig(const QDomNode &configNode)
{
    auto id = parseNumFromNode<u16>(configNode, tags::id);
    auto defVal = parseString(configNode, tags::def_val);
    auto visibility = true;
    if (parseString(configNode, tags::visibility) == "false")
        visibility = false;
    auto count = parseNumFromNode<u16>(configNode, tags::count);
    emit configDataSending(id, defVal, visibility, count);
}

void Xml::ModuleParser::parseHiddenTab(const QDomNode &hiddenTabNode)
{
    auto hiddenTabElem = hiddenTabNode.toElement();
    auto tabTitle = hiddenTabElem.attribute(tags::desc);
    auto tabBackground = hiddenTabElem.attribute(tags::background);
    auto tabPrefix = hiddenTabElem.attribute(tags::prefix);
    auto tabFlag = (hiddenTabElem.attribute(tags::flag)).toUShort();
    std::vector<Xml::HiddenWidget> widgets;
    callForEachChild(hiddenTabNode, [this, &widgets](const QDomNode &hiddenWidgetNode) {
        auto hiddenWidgetElem = hiddenWidgetNode.toElement();
        auto viewStr = hiddenWidgetElem.attribute(tags::view, "LineEdit");
        auto title = hiddenWidgetElem.attribute(tags::title);
        auto view = parseViewType(viewStr);
        auto name = parseString(hiddenWidgetNode, tags::name);
        auto typeStr = parseString(hiddenWidgetNode, tags::type);
        auto type = parseBinaryType(typeStr);
        auto address = parseNumFromNode<u32>(hiddenWidgetNode, tags::addr);
        auto index = parseNumFromNode<u16>(hiddenWidgetNode, tags::index);
        auto visibility = true;
        if (parseString(hiddenWidgetNode, tags::visibility) == "false")
            visibility = false;
        widgets.push_back(Xml::HiddenWidget { name, title, address, index, type, view, visibility });
    });
    emit hiddenTabDataSending(Xml::HiddenTab { tabTitle, tabBackground, tabPrefix, tabFlag, widgets });
}

void Xml::ModuleParser::parseDetector(const QDomNode &node)
{
    const auto tag = node.toElement().tagName();
    if (tag == tags::sigs)
        callForEachChild(node, [this](const QDomNode &sigNode) { parseSignal(sigNode); });
    else if (tag == tags::tabs)
        callForEachChild(node, [this](const QDomNode &sTabNode) { parseSTab(sTabNode); });
    else if (tag == tags::sections)
        callForEachChild(node, [this](const QDomNode &sectionNode) { parseSection(sectionNode); });
    else if (tag == tags::hidden)
        callForEachChild(node, [this](const QDomNode &hiddenTabNode) { parseHiddenTab(hiddenTabNode); });
    else if (tag == tags::alarms)
        parseAlarms(node);
    else if (tag == tags::journals)
        parseJournals(node);
    else if (tag == tags::config)
    {
        auto desc = node.toElement().attribute(tags::desc);
        emit configNameSending(desc);
        callForEachChild(node, [this](const QDomNode &configNode) { parseConfig(configNode); });
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
        // Парсим весь файл, если узлы не указаны требуемые узлы
        if (nodes.empty())
        {
            emit startNewConfig();
            callForEachChild(resourcesNode, parseAction);
            // Настройки интерфейсов всегда парсим отдельно после парсинга сигналов
            parseInterface(resourcesNode);
        }
        else
        {
            for (auto &node : nodes)
                callIfNodeExist(resourcesNode, node, parseAction);
        }
    }
    else
        emit parseError("В файле не найден узел <resources>");
}

void Xml::ModuleParser::parse(const u16 typeB, const u16 typeM, const QStringList &nodes)
{
    auto xmlFilename = getFileName(typeB, typeM);
    auto document = getFileContent(xmlFilename);
    auto moduleNode = document.firstChildElement(tags::module);
    if (!moduleNode.isNull())
    {
        auto resources = moduleNode.firstChildElement(tags::res);
        parseResources(resources, nodes);
    }
    else
        emit parseError("В файле не найден узел <module>");
}

void Xml::ModuleParser::parse(Device::CurrentDevice *device)
{
    if (device)
    {
        const auto typeB = device->getBaseType();
        const auto typeM = device->getMezzType();
        auto xmlFilename = getFileName(typeB, typeM);
        auto document = getFileContent(xmlFilename);
        auto moduleNode = document.firstChildElement(tags::module);
        if (!moduleNode.isNull())
        {
            if (isCorrectDevice(moduleNode, device))
            {
                m_ifaceType = device->async()->getInterfaceType();
                auto resources = moduleNode.firstChildElement(tags::res);
                parseResources(resources);
            }
        }
        else
            emit parseError("В файле не найден узел <module>");
    }
    else
        emit parseError("Получен нулевой указатель на устройство");
}
