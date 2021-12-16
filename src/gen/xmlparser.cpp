#include "xmlparser.h"

#include "../interfaces/iec104.h"
#include "../interfaces/modbus.h"
#include "../interfaces/protocom.h"
#include "board.h"
#include "settings.h"
#include "std_ext.h"

namespace keys
{
constexpr char name[] = "name";
constexpr char stringArray[] = "string-array";
constexpr char uintSet[] = "uint-set";
constexpr char string[] = "string";
constexpr char color[] = "color";
constexpr char unsigned32[] = "quint32";
// constexpr char unsigned64[] = "quint64";
constexpr char unsigned128[] = "quint128";
constexpr char className[] = "class";
constexpr char group[] = "group";
constexpr char type[] = "type";
constexpr char desc[] = "description";
constexpr char toolTip[] = "toolTip";
constexpr char record[] = "record";
constexpr char key[] = "key";
constexpr char value[] = "value";
constexpr char map[] = "map";
}

XmlParser::XmlParser()
{
}

bool XmlParser::isCorrectModule(const QString &typem, const QString &typeb, quint16 m_typem, quint16 m_typeb)
{
    quint16 mtypem = typem.toUShort(nullptr, 16);
    quint16 mtypeb = typeb.toUShort(nullptr, 16);
#ifdef XML_DEBUG
    qDebug() << m_typem << mtypem;
    qDebug() << m_typeb << mtypeb;
#endif
    return (m_typeb == mtypeb && m_typem == mtypem);
}

DataTypes::Alarm XmlParser::parseAlarm(QDomElement domElement)
{
    DataTypes::Alarm alarm;
#ifdef XML_DEBUG
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
#endif
    alarm.name = domElement.attribute(keys::name, "");
    auto element = domElement.firstChildElement(keys::stringArray);
    alarm.desc = parseStringList(element);
    element = domElement.firstChildElement(keys::color);
    alarm.color = element.isNull() ? "" : element.text();

    element = domElement.firstChildElement(keys::unsigned32);
    if (!element.isNull())
    {
        const auto name = element.attribute(keys::name, "");
        if (name.contains("addr", Qt::CaseInsensitive))
            alarm.startAddr = parseInt32(element);
    }

    element = domElement.firstChildElement(keys::unsigned128);
    if (!element.isNull())
    {
        const auto name = element.attribute(keys::name, "");
        if (name.contains("flags", Qt::CaseInsensitive))
            alarm.flags = parseHexInt128(element);
    }

    return alarm;
}

DataTypes::JournalDesc XmlParser::parseJournal(QDomElement domElement)
{
    DataTypes::JournalDesc journal;
#ifdef XML_DEBUG
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
#endif
    journal.name = domElement.attribute(keys::name, "");
    journal.id = parseInt32(domElement.firstChildElement(keys::unsigned32));
    domElement = domElement.firstChildElement(keys::stringArray);
    while (!domElement.isNull())
    {
        const auto name = domElement.attribute(keys::name, "");
        if (name.contains((keys::desc), Qt::CaseInsensitive))
            journal.desc = parseStringList(domElement);
        if (name.contains(("header"), Qt::CaseInsensitive))
            journal.header = parseStringList(domElement);
        domElement = domElement.nextSiblingElement(keys::stringArray);
    }
    return journal;
}

quint32 XmlParser::parseInt32(QDomElement domElement)
{
#ifdef XML_DEBUG
    qDebug() << domElement.attribute("name", "") << domElement.text();
#endif
    if (domElement.text().isEmpty())
        return 0;
    bool ok = false;
    const quint32 number = domElement.text().toUInt(&ok);
    Q_ASSERT(ok);
    return number;
}

quint64 XmlParser::parseHexInt64(QDomElement domElement)
{
    auto str = domElement.text();
#ifdef XML_DEBUG
    qDebug() << domElement.attribute("name", "") << domElement.text();
#endif
    if (domElement.text().isEmpty())
        return 0;
    Q_ASSERT(str.startsWith("0x"));
    str.remove(0, 2);
    bool ok = false;

    const auto number = domElement.text().toULongLong(&ok, 16);
    Q_ASSERT(ok);
    return number;
}

std::bitset<128> XmlParser::parseHexInt128(QDomElement domElement)
{
    auto str = domElement.text();
#ifdef XML_DEBUG
    qDebug() << domElement.attribute("name", "") << domElement.text();
#endif
    if (domElement.text().isEmpty())
        return 0;
    return std::bitset<128>(str.toStdString());
}

QStringList XmlParser::parseStringList(QDomElement domElement)
{
    const auto &nodes = domElement.childNodes();
    QStringList description;
    Q_ASSERT(!nodes.isEmpty());
    int i = 0;
#ifdef XML_DEBUG
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
#endif
    while (i != nodes.count())
    {
        description.push_back(nodes.item(i++).toElement().text());
    }
    return description;
}

class DoubleSpinBoxGroup;
class QLabel;
class QDoubleSpinBox;
class QCheckBox;
class QComboBox;
class QComboBoxGroup;
class QTableView;
class CheckBoxGroup;
class IPCtrl;
class QLineEdit;

ctti::unnamed_type_id_t XmlParser::parseType(QDomElement domElement)
{
    using namespace DataTypes;
    auto name = domElement.text();
    name.replace(" ", "");
    if (name.isEmpty())
        return 0;
    const auto arrSize = name.count('[');
    switch (arrSize)
    {
    case 0:
        // Primitive types
        if (name.contains("BYTE", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<BYTE>().hash();
        if (name.contains("DWORD", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DWORD>().hash();
        if (name.contains("WORD", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD>().hash();
        if (name.contains("INT32", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<INT32>().hash();
        if (name.contains("float", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<float>().hash();
        // 104 types
        if (name.contains("M_SP", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DataTypes::SinglePointWithTimeStruct>().hash();
        if (name.contains("M_BO", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DataTypes::BitStringStruct>().hash();
        if (name.contains("M_ME", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DataTypes::FloatStruct>().hash();

        // Widget classes
        // Group should be checked before single widget
        if (name.contains("Group", Qt::CaseInsensitive))
        {
            if (name.contains("DoubleSpinBoxGroup", Qt::CaseInsensitive))
                return ctti::unnamed_type_id<DoubleSpinBoxGroup>().hash();
            if (name.contains("CheckBoxGroup", Qt::CaseInsensitive))
                return ctti::unnamed_type_id<CheckBoxGroup>().hash();
            if (name.contains("ComboBoxGroup", Qt::CaseInsensitive))
                return ctti::unnamed_type_id<QComboBoxGroup>().hash();
            Q_ASSERT(false && "False type");
        }
        if (name.contains("Label", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<QLabel>().hash();
        if (name.contains("DoubleSpinBox", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<QDoubleSpinBox>().hash();
        if (name.contains("IpControl", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<IPCtrl>().hash();
        if (name.contains("CheckBox", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<QCheckBox>().hash();
        if (name.contains("ComboBox", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<QComboBox>().hash();
        if (name.contains("LineEdit", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<QLineEdit>().hash();
        // Another types
        if (name.contains("TableView", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<QTableView>().hash();

        [[fallthrough]];
    case 1:

        if (name.contains("BYTE[4]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<BYTE_4t>().hash();
        if (name.contains("BYTE[8]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<BYTE_8t>().hash();
        if (name.contains("BYTE[16]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<BYTE_16t>().hash();
        if (name.contains("BYTE[32]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<BYTE_32t>().hash();
        if (name.contains("DWORD[8]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DWORD_8t>().hash();
        if (name.contains("DWORD[16]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DWORD_16t>().hash();
        if (name.contains("DWORD[32]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DWORD_32t>().hash();
        if (name.contains("WORD[4]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD_4t>().hash();
        if (name.contains("WORD[8]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD_8t>().hash();
        if (name.contains("WORD[16]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD_16t>().hash();
        if (name.contains("WORD[32]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD_32t>().hash();
        if (name.contains("float[2]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_2t>().hash();
        if (name.contains("float[3]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_3t>().hash();
        if (name.contains("float[4]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_4t>().hash();
        if (name.contains("float[6]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_6t>().hash();
        if (name.contains("float[8]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_8t>().hash();

        [[fallthrough]];
    default:
        assert(false && "Unknown type");
    }
    return 0;
}

config::itemVariant XmlParser::parseWidget(QDomElement domElement)
{
    auto name = domElement.text();
#ifdef XML_DEBUG
    qDebug() << name;
#endif
    QString className = domElement.attribute(keys::className);
    auto type = parseType(domElement.firstChildElement(keys::type));

    if (!className.isEmpty())
    {
        return parseItem(domElement, type);
    }
    QStringList items;

    QDomElement childElement = domElement.firstChildElement(keys::stringArray);
    if (!childElement.isNull())
        items = parseStringList(childElement);

    childElement = domElement.firstChildElement(keys::group);
    auto widgetGroup = static_cast<delegate::WidgetGroup>(childElement.text().toInt());

    const QString description = domElement.firstChildElement(keys::string).text();
    const QString toolTip = domElement.firstChildElement(keys::toolTip).text();

    switch (type.hash())
    {
    case ctti::unnamed_type_id<QDoubleSpinBox>().hash():
    {
        bool status = false;
        delegate::DoubleSpinBoxWidget widget(type, widgetGroup);

        QDomElement nextChildElement = domElement.firstChildElement("min");
        widget.min = nextChildElement.text().toDouble(&status);
        nextChildElement = domElement.firstChildElement("max");
        widget.max = nextChildElement.text().toDouble(&status);
        nextChildElement = domElement.firstChildElement("decimals");
        widget.decimals = nextChildElement.text().toUInt(&status);
        if (!status)
            qWarning() << name << className;

        widget.desc = description;
        widget.toolTip = toolTip;
        return std::move(widget);
    }
    case ctti::unnamed_type_id<DoubleSpinBoxGroup>().hash():
    {
        bool status = false;
        delegate::DoubleSpinBoxGroup widget(type, widgetGroup);

        QDomElement nextChildElement = domElement.firstChildElement("min");
        widget.min = nextChildElement.text().toDouble(&status);
        nextChildElement = domElement.firstChildElement("max");
        widget.max = nextChildElement.text().toDouble(&status);
        nextChildElement = domElement.firstChildElement("decimals");
        widget.decimals = nextChildElement.text().toUInt(&status);
        nextChildElement = domElement.firstChildElement("count");
        widget.count = nextChildElement.text().toUInt(&status);
        if (!status)
            qWarning() << name << className;

        widget.desc = description;
        widget.toolTip = toolTip;
        widget.items = items;
        return std::move(widget);
    }
    case ctti::unnamed_type_id<CheckBoxGroup>().hash():
    {
        bool status = false;
        delegate::CheckBoxGroup widget(type, widgetGroup);

        QDomElement nextChildElement = domElement.firstChildElement("count");
        widget.count = nextChildElement.text().toUInt(&status);
        if (!status)
            qWarning() << name << className;

        widget.desc = description;
        widget.toolTip = toolTip;
        widget.items = items;
        return std::move(widget);
    }
    case ctti::unnamed_type_id<QComboBox>().hash():
    {
        delegate::QComboBox widget(type, widgetGroup);

        widget.desc = description;
        widget.toolTip = toolTip;
        widget.model = items;
        QDomElement nextChildElement = domElement.firstChildElement("field");
        // QComboBox depends on index by default
        if (nextChildElement.text().contains("data"))
            widget.primaryField = delegate::QComboBox::data;
        else if (nextChildElement.text().contains("bitfield"))
            widget.primaryField = delegate::QComboBox::bitfield;
        else
            widget.primaryField = delegate::QComboBox::index;
        //    = childElement.text().contains("data") ? delegate::QComboBox::data : delegate::QComboBox::index;
        return std::move(widget);
    }
    case ctti::unnamed_type_id<QComboBoxGroup>().hash():
    {
        delegate::QComboBoxGroup widget(type, widgetGroup);

        widget.desc = description;
        widget.toolTip = toolTip;
        widget.model = items;
        QDomElement nextChildElement = domElement.firstChildElement("field");
        // QComboBox depends on index by default
        if (nextChildElement.text().contains("data"))
            widget.primaryField = delegate::QComboBox::data;
        else if (nextChildElement.text().contains("bitfield"))
            widget.primaryField = delegate::QComboBox::bitfield;
        else
            widget.primaryField = delegate::QComboBox::index;
        //    = childElement.text().contains("data") ? delegate::QComboBox::data : delegate::QComboBox::index;
        nextChildElement = domElement.firstChildElement("count");
        bool status = false;
        widget.count = nextChildElement.text().toUInt(&status);
        if (!status)
            qWarning() << name << className;
        return std::move(widget);
    }
    default:
    {
    }
    }
    return delegate::Widget(type, description, widgetGroup, toolTip);
}

void XmlParser::mergeWidget(const QDomElement &domElement, config::widgetMap *const s2widgetMap, BciNumber id)
{
    using namespace config;
    auto newWidget = parseWidget(domElement);
    auto oldWidget = s2widgetMap->at(id);
    newWidget = std::visit(
        [&](auto &&lhs, auto &&rhs) -> itemVariant {
            using T = std::decay_t<decltype(lhs)>;
            using J = std::decay_t<decltype(rhs)>;

            if constexpr (std::is_same_v<T, J>)
            {
                if constexpr (std::is_same_v<T, Item>)
                {
                    abort();
                }
                return lhs.merge(rhs);
            }
            abort();
        },
        oldWidget, newWidget);
    s2widgetMap->insert_or_assign(id, newWidget);
}

DataTypes::RecordPair XmlParser::parseRecordConfig(QDomElement domElement, config::widgetMap *const s2widgetMap)
{
    using namespace DataTypes;
    QDomElement childElement = domElement.firstChildElement("id");
    // ID cannot be empty
    if (childElement.isNull())
        return {};
    bool status = false;
    unsigned id = childElement.text().toUInt(&status);
    // ID should be convertible to int
    if (!status)
        return {};

    // override settings
    auto widgetElement = domElement.firstChildElement("widget");
    if (!widgetElement.isNull())
    {
        mergeWidget(widgetElement, s2widgetMap, BciNumber(id));
    }

    auto defaultValueElement = domElement.firstChildElement("defaultValue");
    // defaultValue is necessary, is this ok?
    if (defaultValueElement.isNull())
        return {};
    auto defaultValue = defaultValueElement.text();

    auto visibilityElement = domElement.firstChildElement("visibility");
    // visibility=true by default
    if (visibilityElement.isNull() || visibilityElement.text() == "true")
        return RecordPair { DataTypes::DataRecV(id, defaultValue), true };
    if (visibilityElement.text() == "false")
        return RecordPair { DataTypes::DataRecV(id, defaultValue), false };
    Q_ASSERT(false && "Wrong visible value: " && defaultValue.toStdString().c_str());
    return RecordPair { DataTypes::DataRecV(id, defaultValue), true };
}

check::detail::Record XmlParserHelper::parseRecordCheck(QDomElement domElement)
{
    check::detail::Record rec;
    bool ok = false;

    auto header = domElement.attribute("header");
    rec.header = header;

    auto groupElement = domElement.firstChildElement(keys::group);
    if (!groupElement.isNull())
    {
        auto group = groupElement.text().toUShort(&ok);
        rec.group = group;
        assert(ok);
    }

    rec.start_addr = domElement.firstChildElement("start-addr").text().toUShort(&ok);
    assert(ok);
    auto countElement = domElement.firstChildElement("count");
    if (!countElement.isNull())
    {
        rec.count = domElement.firstChildElement("count").text().toUShort(&ok);
        assert(ok);
    }

    QString toolTip;
    auto toolTipElement = domElement.firstChildElement(keys::toolTip);
    if (toolTipElement.isElement())
        toolTip = toolTipElement.text();

    auto desc = domElement.attribute("desc");

    if (!rec.count.has_value())
    {
        rec.desc = QStringList(desc);
        if (!toolTip.isEmpty())
            rec.toolTip = QStringList(toolTip);
    }
    else
    {
        auto strArrayElement = domElement.firstChildElement(keys::stringArray);
        QStringList strDesc, strToolTip;
        if (strArrayElement.isElement())
        {
            auto strList = XmlParser::parseStringList(strArrayElement);
            assert(rec.count == strList.count());
            for (auto &&item : strList)
            {
                if (toolTip.isEmpty())
                    strToolTip.push_back(item);
                else
                    strToolTip.push_back(toolTip.arg(item));
                if (!desc.isEmpty())
                    strDesc.push_back(desc.arg(item));
            }
        }
        else
        {
            for (auto i = 1; i <= rec.count; i++)
            {
                if (!toolTip.isEmpty())
                    strToolTip.push_back(toolTip.arg(i));
                if (!desc.isEmpty())
                    strDesc.push_back(desc.arg(i));
            }
        }
        if (!toolTip.isEmpty())
            rec.toolTip = strToolTip;

        rec.desc = strDesc;
    }

    return rec;
}

check::detail::Signals XmlParserHelper::parseSignals(QDomElement domElement)
{
    check::detail::Signals signls;
    auto typeElement = domElement.firstChildElement(keys::type);
    if (typeElement.isNull())
        throw std::runtime_error("null element");

    signls.type = XmlParser::parseType(typeElement);
    auto vectorElement = domElement.firstChildElement(keys::uintSet);
    if (typeElement.isNull())
        throw std::runtime_error("null element");

    signls.groups = XmlParser::parseSet<uint16_t>(vectorElement);
    auto addrElement = domElement.firstChildElement("start-addr");
    bool ok = false;
    if (addrElement.isNull())
        throw std::runtime_error("null element");

    signls.start_addr = addrElement.text().toUShort(&ok);
    assert(ok);

    auto countElement = domElement.firstChildElement("count");
    if (countElement.isNull())
        throw std::runtime_error("null element");

    signls.count = countElement.text().toUShort(&ok);
    assert(ok);

    return signls;
}

check::itemVariant XmlParser::parseRecordCheck(QDomElement domElement)
{
    auto record = domElement.firstChildElement(keys::record);
    if (record.isNull())
        return XmlParserHelper::parseRecordCheck(domElement);

    check::detail::RecordList recordList;

    auto group = domElement.firstChildElement(keys::group).text().toUShort();
    recordList.group = group;
    auto header = domElement.attribute("header");
    recordList.header = header;
    while (!record.isNull())
    {
        recordList.records.emplace_back(XmlParserHelper::parseRecordCheck(record));
        record = record.nextSiblingElement(keys::record);
    }
    return std::move(recordList);
}

config::Item XmlParser::parseItem(QDomElement domElement, ctti::unnamed_type_id_t parentType)
{
    using namespace config;
    auto name = domElement.text();
#ifdef XML_DEBUG
    qDebug() << name;
#endif
    QString className = domElement.attribute(keys::className);
    if (className.isEmpty())
        return { 0 };
    delegate::ItemType itemType;
    if (className == "ModbusItem")
        itemType = delegate::ItemType::ModbusItem;
    else
        return { 0 };

    QDomElement childElement = domElement.firstChildElement(keys::group);
    auto widgetGroup = static_cast<delegate::WidgetGroup>(childElement.text().toInt());
    switch (itemType)
    {
    case delegate::ItemType::ModbusItem:
    {

        QDomElement nextChildElement = domElement.firstChildElement("parent");
        bool status = false;
        auto parent = static_cast<BciNumber>(nextChildElement.text().toUInt(&status));
        if (!status)
            qWarning() << name << className;
        Item item(parentType, itemType, parent, widgetGroup);
        return item;
    }
    default:
        return Item(parentType, itemType, BciNumber::dummyElement, widgetGroup);
    }
}

void XmlParser::traverseNode(const QDomNode &node, ModuleSettings *const settings, ConfigSettings &configSettings)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == keys::unsigned32)
                {
                    XmlParser::parseInt32(domElement);
                }
                if (domElement.tagName() == keys::stringArray)
                {
#ifdef XML_DEBUG
                    qDebug() << "Attr: " << domElement.attribute("name", "");
#endif
                    XmlParser::parseStringList(domElement);
                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "alarm")
                {
                    using namespace Modules;
                    qDebug() << "Attr: " << domElement.attribute(keys::name, "");
                    const auto alarm = XmlParser::parseAlarm(domElement);
                    if (alarm.name.contains("critical", Qt::CaseInsensitive))
                        settings->alarms.insert(AlarmType::Critical, alarm);
                    else if (alarm.name.contains("warning", Qt::CaseInsensitive))
                        settings->alarms.insert(AlarmType::Warning, alarm);
                    else
                        settings->alarms.insert(AlarmType::All, alarm);

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "journal")
                {
#ifdef XML_DEBUG
                    qDebug() << "Attr: " << domElement.attribute("name", "");
#endif
                    using namespace Modules;
                    const auto journal = XmlParser::parseJournal(domElement);
                    if (journal.name.contains("work", Qt::CaseInsensitive))
                        settings->journals.insert(JournalType::Work, journal);
                    else if (journal.name.contains("meas", Qt::CaseInsensitive))
                        settings->journals.insert(JournalType::Meas, journal);
                    else if (journal.name.contains("sys", Qt::CaseInsensitive))
                        settings->journals.insert(JournalType::System, journal);

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "module")
                {
                    if (!isCorrectModule(domElement.attribute("mtypem", ""), domElement.attribute("mtypeb", ""),
                            settings->startupInfoBlock.MTypeM, settings->startupInfoBlock.MTypeB))
                    {
                        domNode = domNode.nextSibling();
                        continue;
                    }
                }

                if (domElement.tagName() == "version")
                {
                    if (settings->startupInfoBlock.isOutdated(StdFunc::StrToVer(domElement.text())))
                    {
                        throw std::runtime_error("Устаревшая версия ВПО, обновите ВПО");
                    }
                }

                if (domElement.tagName() == "multimap")
                {
                    if (domElement.attribute("name").contains("warn", Qt::CaseInsensitive))
                        settings->highlightWarn = parseMap<QMultiMap<quint32, quint32>>(domElement);
                    else if (domElement.attribute("name").contains("crit", Qt::CaseInsensitive))
                        settings->highlightCrit = parseMap<QMultiMap<quint32, quint32>>(domElement);
                }

                if (domElement.tagName() == "modbus")
                {
                    if (settings->interfaceType == Board::RS485)
                    {
                        ModBus interface;
                        settings->ifaceSettings = interface.parseSettings(domElement);
                    }

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "protocom")
                {
                    if (settings->interfaceType == Board::USB || settings->interfaceType == Board::Emulator)
                    {
                        Protocom interface;
                        if (settings->ifaceSettings.settings.isNull())
                            settings->ifaceSettings = interface.parseSettings(domElement);
                        else if (settings->ifaceSettings.settings.canConvert<InterfaceInfo<Proto::ProtocomGroup>>())
                        {
                            auto oldSettings
                                = settings->ifaceSettings.settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
                            auto newSettings = interface.parseSettings(domElement)
                                                   .settings.value<InterfaceInfo<Proto::ProtocomGroup>>();
                            oldSettings.merge(newSettings);
                            settings->ifaceSettings = { QVariant::fromValue(oldSettings) };
                        }
                    }

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "iec60870")
                {
                    if (settings->interfaceType == Board::Ethernet)
                    {
                        IEC104 interface;
                        settings->ifaceSettings = interface.parseSettings(domElement);
                    }

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == keys::record)
                {

                    settings->configSettings.general.push_back(
                        parseRecordConfig(domElement, configSettings.s2widgetMap));
                    domNode = domNode.nextSibling();
                    continue;
                }
            }
        }
        traverseNode(domNode, settings, configSettings);
        domNode = domNode.nextSibling();
    }
}

void XmlParser::traverseNode(const QDomNode &node, ConfigSettings &settings)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == keys::record)
                {
#ifdef XML_DEBUG
                    qDebug() << domElement.text();
#endif
                    domElement = domElement.firstChild().toElement();
                    BciNumber id = static_cast<BciNumber>(XmlParser::parseInt32(domElement));
                    domElement = domElement.nextSibling().toElement();
                    settings.s2filesMap->insert(id, parseType(domElement));
                    domElement = domElement.nextSibling().toElement();
                    settings.s2widgetMap->insert({ id, parseWidget(domElement) });

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == keys::map)
                {
                    *settings.s2categories = parseMap<categoryMap>(domElement);
                    qDebug() << *settings.s2categories;
                }
            }
        }
        traverseNode(domNode, settings);
        domNode = domNode.nextSibling();
    }
}

void XmlParser::traverseNode(const QDomNode &node, categoryMap &settings)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == keys::map)
                {
                    settings = parseMap<categoryMap>(domElement);
                    qDebug() << settings;
                }
            }
        }
        traverseNode(domNode, settings);
        domNode = domNode.nextSibling();
    }
}

void XmlParser::traverseNodeS2(const QDomNode &node, QList<DataTypes::RecordPair> &settings, config::widgetMap *widgets)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == keys::unsigned32)
                {
                    XmlParser::parseInt32(domElement);
                }
                if (domElement.tagName() == keys::stringArray)
                {
#ifdef XML_DEBUG
                    qDebug() << "Attr: " << domElement.attribute("name", "");
#endif
                    XmlParser::parseStringList(domElement);
                    domNode = domNode.nextSibling();
                    continue;
                }

                if (domElement.tagName() == keys::record)
                {

                    settings.push_back(parseRecordConfig(domElement, widgets));
                    domNode = domNode.nextSibling();
                    continue;
                }
            }
        }
        traverseNodeS2(domNode, settings, widgets);
        domNode = domNode.nextSibling();
    }
}

void XmlParser::traverseNodeCheck(const QDomNode &node, std::vector<CheckItem> &settings)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {

                if (domElement.tagName() == "check")
                {

                    settings.emplace_back(traverseNodeCheck(domElement));
                    domNode = domNode.nextSibling();
                    continue;
                }
            }
        }
        traverseNodeCheck(domNode, settings);
        domNode = domNode.nextSibling();
    }
}

DataTypes::Alarm XmlParser::traverseNodeAlarm(const QDomNode &node)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == "alarm")
                {

                    return parseAlarm(domElement);
                }
            }
        }
        traverseNodeAlarm(domNode);
        domNode = domNode.nextSibling();
    }
    assert(false);
    return DataTypes::Alarm();
}

CheckItem XmlParser::traverseNodeCheck(const QDomNode &node)
{
    CheckItem checkItem;
    QString header = node.toElement().attribute("header");
    assert(!header.isEmpty());
    checkItem.header = header;
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == "signals")
                {
                    checkItem.signlsVec.emplace_back(XmlParserHelper::parseSignals(domElement));
                    domNode = domNode.nextSibling();
                    continue;
                }

                if (domElement.tagName() == keys::record)
                {

                    checkItem.itemsVector.emplace_back(parseRecordCheck(domElement));
                    domNode = domNode.nextSibling();
                    continue;
                }
            }
        }
        //   traverseNodeCheck(domNode, settings);
        domNode = domNode.nextSibling();
    }
    return checkItem;
}

template <typename T> std::vector<T> XmlParser::parseVector(QDomElement domElement)
{
    const auto &nodes = domElement.childNodes();
    std::vector<T> vector;
    Q_ASSERT(!nodes.isEmpty());
    int i = 0;
#ifdef XML_DEBUG
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
#endif
    while (i != nodes.count())
    {
        vector.push_back(QVariant(nodes.item(i++).toElement().text()).value<T>());
    }
    return vector;
}

template <typename T> std::set<T> XmlParser::parseSet(QDomElement domElement)
{
    const auto &nodes = domElement.childNodes();
    std::set<T> set;
    Q_ASSERT(!nodes.isEmpty());
    int i = 0;
#ifdef XML_DEBUG
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
#endif
    while (i != nodes.count())
    {
        set.insert(QVariant(nodes.item(i++).toElement().text()).value<T>());
    }
    return set;
}

template <typename Container> Container XmlParser::parseMap(QDomElement domElement)
{
    Container map;
    using Key = typename decltype(map)::key_type;
    using Value = typename decltype(map)::mapped_type;
    if (domElement.isNull())
        return map;

    QDomNode domNode = domElement.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == "item")
                {
#ifdef XML_DEBUG
                    qDebug() << domElement.text();
#endif
                    // domElement = domElement.firstChild().toElement();
                    auto nodes = domElement.childNodes();
                    Key key;
                    int i = 0;
                    while (i != nodes.count())
                    {
                        domElement = nodes.item(i++).toElement();
                        if (domElement.tagName() == keys::key)
                        {
                            key = QVariant(domElement.text()).value<Key>();
                        }
                        else if (domElement.tagName() == keys::value)
                        {
                            if constexpr (std::is_same_v<QString, Value>)
                            {
                                Value value = domElement.text();
                                map.insert(key, value);
                            }
                            else
                            {
                                Value value = QVariant(domElement.text()).value<Value>();
                                map.insert(key, value);
                            }
                        }
                    }

                    domNode = domNode.nextSibling();
                    continue;
                }
            }
        }

        domNode = domNode.nextSibling();
    }
    return map;
}
