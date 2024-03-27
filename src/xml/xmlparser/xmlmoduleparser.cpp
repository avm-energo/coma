#include "xmlmoduleparser.h"

#include <gen/stdfunc.h>

Xml::ModuleParser::ModuleParser(const quint16 typeB, const quint16 typeM, const bool check, QObject *parent)
    : BaseParser(parent)
{
    auto xmlFilename = getFileName(typeB, typeM);
    auto document = getFileContent(xmlFilename);
    auto moduleNode = document.firstChildElement(tags::module);
    if (!moduleNode.isNull())
    {
        auto checkResult = true;
        if (check)
            checkResult = isCorrectModule(moduleNode, typeB, typeM);
        if (checkResult)
            resources = moduleNode.firstChildElement(tags::res);
    }
}

/// \brief Получаем имя файла по типам базовой и мезонинной плат.
QString Xml::ModuleParser::getFileName(const quint16 typeB, const quint16 typeM)
{
    auto typeBStr = QString::number(typeB, 16);
    typeBStr = (typeBStr.length() < 2) ? "0" + typeBStr : typeBStr;
    auto typeMStr = QString::number(typeM, 16);
    typeMStr = (typeMStr.length() < 2) ? "0" + typeMStr : typeMStr;
    return typeBStr + typeMStr + ".xml";
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
        // Проверяем корректность версии только для базы
        auto isBase = (typeB == 0) ? false : true;
        if (isBase)
        {
            if (isCorrectModuleVersion(moduleNode))
                return true;
            else
                qCritical() << "Устаревшая версия ВПО, обновите ВПО";
        }
        else
            return true;
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

/// \brief Функция для определения типа отображения мультивиджета.
ModuleTypes::ViewType Xml::ModuleParser::parseViewType(const QString &viewString)
{
    if (viewString.contains("bitset", Qt::CaseInsensitive))
        return ModuleTypes::ViewType::Bitset;
    else if (viewString.contains("LineEdit", Qt::CaseInsensitive))
        return ModuleTypes::ViewType::LineEdit;
    else if (viewString.contains("Version", Qt::CaseInsensitive))
        return ModuleTypes::ViewType::Version;
    else
        return ModuleTypes::ViewType::Float;
}

/// \brief Функция для определения типа отображаемых/отправляемых данных.
ModuleTypes::BinaryType Xml::ModuleParser::parseBinaryType(const QString &typeStr)
{
    if (typeStr == "uint32")
        return ModuleTypes::BinaryType::uint32;
    else if (typeStr == "time32")
        return ModuleTypes::BinaryType::time32;
    else if (typeStr == "time64")
        return ModuleTypes::BinaryType::time64;
    else
        return ModuleTypes::BinaryType::float32;
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
            auto mwidgetElem = mwidgetNode.toElement();
            auto mwidgetDesc = mwidgetElem.attribute(tags::desc, "");
            auto viewString = mwidgetElem.attribute(tags::view, "float");
            auto addr = parseNumFromNode<quint32>(mwidgetNode, tags::start_addr);
            auto count = parseNumFromNode<quint32>(mwidgetNode, tags::count);
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
    parseNode(joursNode, tags::work, [this](const QDomNode &jourNode) { parseWorkJournal(jourNode); });
    parseNode(joursNode, tags::meas, [this](const QDomNode &jourNode) { parseMeasJournal(jourNode); });
}

/// \brief Функция для парсинга узла <work> внутри <journals>.
void Xml::ModuleParser::parseWorkJournal(const QDomNode &jourNode)
{
    auto id = parseNumFromNode<quint32>(jourNode, tags::addr);
    auto desc = parseString(jourNode, tags::desc);
    emit workJourDataSending(id, desc);
}

/// \brief Функция для парсинга узла <meas> внутри <journals>.
void Xml::ModuleParser::parseMeasJournal(const QDomNode &jourNode)
{
    auto index = parseNumFromNode<quint32>(jourNode, tags::index);
    auto header = parseString(jourNode, tags::header);
    auto strType = parseString(jourNode, tags::type);
    ModuleTypes::BinaryType type = parseBinaryType(strType);
    auto visibility = true;
    if (parseString(jourNode, tags::visibility) == "false")
        visibility = false;
    emit measJourDataSending(index, header, type, visibility);
}

/// \brief Функция для парсинга конфигурации интерфейса, по которому подключен модуль.
void Xml::ModuleParser::parseInterface(const QDomNode &resNode)
{
    auto ifaceType = Board::GetInstance().interfaceType();
    switch (ifaceType)
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
        qCritical() << "Undefined interface type";
        return;
    }
}

/// \brief Функция для парсинга узла <modbus>.
void Xml::ModuleParser::parseModbus(const QDomNode &modbusNode)
{
    auto signalId = parseNumFromNode<quint32>(modbusNode, tags::sig_id);
    auto regType = parseNumFromNode<quint16>(modbusNode, tags::reg_type);
    if (signalId != 0)
    {
        Protocol::AbstractGroup str { Interface::IfaceType::RS485, signalId, regType, quint16() };
        emit protocolGroupSending(str);
    }
}

/// \brief Функция для парсинга узла <protocom>.
void Xml::ModuleParser::parseProtocom(const QDomNode &protocomNode)
{
    auto signalId = parseNumFromNode<quint32>(protocomNode, tags::sig_id);
    auto block = parseNumFromNode<quint16>(protocomNode, tags::block);
    if (signalId != 0)
    {
        Protocol::AbstractGroup str { Interface::IfaceType::USB, signalId, block, quint16() };
        emit protocolGroupSending(str);
    }
}

/// \brief Функция для парсинга узла <iec60870>.
void Xml::ModuleParser::parseIec(const QDomNode &iecNode)
{
    auto signalId = parseNumFromNode<quint32>(iecNode, tags::sig_id);
    auto transType = parseNumFromNode<quint16>(iecNode, tags::trans_type);
    auto sigGroup = parseNumFromNode<quint16>(iecNode, tags::sig_group);
    if (signalId != 0)
    {
        Protocol::AbstractGroup str { Interface::IfaceType::Ethernet, signalId, transType, sigGroup };
        emit protocolGroupSending(str);
    }
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

/// \brief Функция для парсинга узлов <tab> внутри <hidden>.
void Xml::ModuleParser::parseHiddenTab(const QDomNode &hiddenTabNode)
{
    auto hiddenTabElem = hiddenTabNode.toElement();
    auto tabTitle = hiddenTabElem.attribute(tags::desc);
    auto tabBackground = hiddenTabElem.attribute(tags::background);
    auto tabPrefix = hiddenTabElem.attribute(tags::prefix);
    auto tabFlag = (hiddenTabElem.attribute(tags::flag)).toUShort();
    std::vector<ModuleTypes::HiddenWidget> widgets;
    callForEachChild(hiddenTabNode, [this, &widgets](const QDomNode &hiddenWidgetNode) {
        auto hiddenWidgetElem = hiddenWidgetNode.toElement();
        auto viewStr = hiddenWidgetElem.attribute(tags::view, "LineEdit");
        auto title = hiddenWidgetElem.attribute(tags::title);
        auto view = parseViewType(viewStr);
        auto name = parseString(hiddenWidgetNode, tags::name);
        auto typeStr = parseString(hiddenWidgetNode, tags::type);
        auto type = parseBinaryType(typeStr);
        auto address = parseNumFromNode<quint32>(hiddenWidgetNode, tags::addr);
        auto index = parseNumFromNode<quint16>(hiddenWidgetNode, tags::index);
        auto visibility = true;
        if (parseString(hiddenWidgetNode, tags::visibility) == "false")
            visibility = false;
        widgets.push_back(ModuleTypes::HiddenWidget { name, title, address, index, type, view, visibility });
    });
    emit hiddenTabDataSending(ModuleTypes::HiddenTab { tabTitle, tabBackground, tabPrefix, tabFlag, widgets });
}

/// \brief Функция для парсинга узла <resources>.
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

/// \brief Функция для парсинга файла конфигурации модуля.
void Xml::ModuleParser::parse(const QStringList &nodes)
{
    if (!resources.isNull())
    {
        auto parseAction = [this](const QDomNode &node) { parseDetector(node); };
        // Парсим весь файл, если узлы не указаны требуемые узлы
        if (nodes.empty())
        {
            emit startNewConfig();
            callForEachChild(resources, parseAction);
            // Настройки интерфейсов всегда парсим отдельно после парсинга сигналов
            parseInterface(resources);
        }
        else
        {
            for (auto &node : nodes)
                callIfNodeExist(resources, node, parseAction);
        }
    }
}
