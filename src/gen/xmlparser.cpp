#include "xmlparser.h"

//#include "../module/module.h"
//#include "../widgets/checkboxgroup.h"

#include "../interfaces/baseinterface.h"
#include "board.h"
#include "settings.h"
#include "std_ext.h"
namespace keys
{
constexpr char name[] = "name";
constexpr char stringArray[] = "string-array";
constexpr char string[] = "string";
constexpr char color[] = "color";
constexpr char unsigned32[] = "quint32";
constexpr char unsigned64[] = "quint64";
constexpr char className[] = "class";
constexpr char group[] = "group";
constexpr char type[] = "type";
constexpr char desc[] = "description";
constexpr char toolTip[] = "toolTip";
}

XmlParser::XmlParser()
{
}

bool isCorrectModule(const QString &typem, const QString &typeb)
{
    const auto &board = Board::GetInstance();
    quint16 mtypem = typem.toUInt(nullptr, 16);
    quint16 mtypeb = typeb.toUInt(nullptr, 16);
#ifdef XML_DEBUG
    qDebug() << typem << mtypem;
    qDebug() << typem << mtypeb;
#endif
    if (board.typeB() != mtypeb)
        return false;
    if (board.typeM() != mtypem)
        return false;
    return true;
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

    element = domElement.firstChildElement(keys::unsigned64);
    if (!element.isNull())
    {
        const auto name = element.attribute(keys::name, "");
        if (name.contains("flags", Qt::CaseInsensitive))
            alarm.flags = parseHexInt64(element);

        //        element = element.nextSiblingElement(keys::unsigned32);

        //        if (name.contains("addr", Qt::CaseInsensitive))
        //            alarm.startAddr = parseInt32(element);
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
        if (name.contains("float[6]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_6t>().hash();
        if (name.contains("float[8]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_8t>().hash();

    case 2:
        if (name.contains("float[2][2]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_2t_2t>().hash();

    default:
        assert(false && "Unknown type");
    }
    return 0;
}

delegate::itemVariant XmlParser::parseWidget(QDomElement domElement)
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

        QDomElement childElement = domElement.firstChildElement("min");
        widget.min = childElement.text().toDouble(&status);
        childElement = domElement.firstChildElement("max");
        widget.max = childElement.text().toDouble(&status);
        childElement = domElement.firstChildElement("decimals");
        widget.decimals = childElement.text().toUInt(&status);
        if (!status)
            qWarning() << name << className;

        widget.desc = description;
        widget.toolTip = toolTip;
        return widget;
    }
    case ctti::unnamed_type_id<DoubleSpinBoxGroup>().hash():
    {
        bool status = false;
        delegate::DoubleSpinBoxGroup widget(type, widgetGroup);

        QDomElement childElement = domElement.firstChildElement("min");
        widget.min = childElement.text().toDouble(&status);
        childElement = domElement.firstChildElement("max");
        widget.max = childElement.text().toDouble(&status);
        childElement = domElement.firstChildElement("decimals");
        widget.decimals = childElement.text().toUInt(&status);
        childElement = domElement.firstChildElement("count");
        widget.count = childElement.text().toUInt(&status);
        if (!status)
            qWarning() << name << className;

        widget.desc = description;
        widget.toolTip = toolTip;
        widget.items = items;
        return widget;
    }
    case ctti::unnamed_type_id<CheckBoxGroup>().hash():
    {
        bool status = false;
        delegate::CheckBoxGroup widget(type, widgetGroup);

        QDomElement childElement = domElement.firstChildElement("count");
        widget.count = childElement.text().toUInt(&status);
        if (!status)
            qWarning() << name << className;

        widget.desc = description;
        widget.toolTip = toolTip;
        widget.items = items;
        return widget;
    }
    case ctti::unnamed_type_id<QComboBox>().hash():
    {
        delegate::QComboBox widget(type, widgetGroup);

        widget.desc = description;
        widget.toolTip = toolTip;
        widget.model = items;
        QDomElement childElement = domElement.firstChildElement("field");
        // QComboBox depends on index by default
        if (childElement.text().contains("data"))
            widget.primaryField = delegate::QComboBox::data;
        else if (childElement.text().contains("bitfield"))
            widget.primaryField = delegate::QComboBox::bitfield;
        else
            widget.primaryField = delegate::QComboBox::index;
        //    = childElement.text().contains("data") ? delegate::QComboBox::data : delegate::QComboBox::index;
        return widget;
    }
    case ctti::unnamed_type_id<QComboBoxGroup>().hash():
    {
        delegate::QComboBoxGroup widget(type, widgetGroup);

        widget.desc = description;
        widget.toolTip = toolTip;
        widget.model = items;
        QDomElement childElement = domElement.firstChildElement("field");
        // QComboBox depends on index by default
        if (childElement.text().contains("data"))
            widget.primaryField = delegate::QComboBox::data;
        else if (childElement.text().contains("bitfield"))
            widget.primaryField = delegate::QComboBox::bitfield;
        else
            widget.primaryField = delegate::QComboBox::index;
        //    = childElement.text().contains("data") ? delegate::QComboBox::data : delegate::QComboBox::index;
        childElement = domElement.firstChildElement("count");
        bool status = false;
        widget.count = childElement.text().toUInt(&status);
        if (!status)
            qWarning() << name << className;
        return widget;
    }
    default:
    {
    }
    }
    return delegate::Widget(type, description, widgetGroup, toolTip);
}

DataTypes::RecordPair XmlParser::parseRecord(QDomElement domElement, widgetMap *const s2widgetMap)
{
    using namespace DataTypes;
    QDomElement childElement = domElement.firstChildElement("id");
    // ID cannot be empty
    if (childElement.isNull())
        return {};
    bool status = false;
    int id = childElement.text().toInt(&status);
    // ID should be convertible to int
    if (!status)
        return {};

    childElement = domElement.firstChildElement("defaultValue");
    // defaultValue is necessary, is this ok?
    if (childElement.isNull())
        return {};
    // override settings
    childElement = domElement.firstChildElement("widget");
    if (!childElement.isNull())
    {
        auto newWidget = parseWidget(childElement);
        auto oldWidget = s2widgetMap->at(BciNumber(id));
        newWidget = std::visit(
            [&](auto &&lhs, auto &&rhs) -> delegate::itemVariant {
                using T = std::decay_t<decltype(lhs)>;
                using J = std::decay_t<decltype(rhs)>;

                if constexpr (std::is_same_v<T, J>)
                {
                    if constexpr (std::is_same_v<T, delegate::Item>)
                    {
                        abort();
                    }
                    return lhs.merge(rhs);
                }
                abort();
            },
            oldWidget, newWidget);
        s2widgetMap->insert_or_assign(BciNumber(id), newWidget);
    }

    auto visibilityElement = domElement.firstChildElement("visibility");
    // visibility=true by default
    if (visibilityElement.isNull() || visibilityElement.text() == "true")
        return RecordPair { DataTypes::DataRecV(id, childElement.text()), true };
    if (visibilityElement.text() == "false")
        return RecordPair { DataTypes::DataRecV(id, childElement.text()), false };
    Q_ASSERT(false && "Wrong visible value: " && childElement.text().toStdString().c_str());
    return RecordPair { DataTypes::DataRecV(id, childElement.text()), true };
}

delegate::Item XmlParser::parseItem(QDomElement domElement, ctti::unnamed_type_id_t parentType)
{
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

        QDomElement childElement = domElement.firstChildElement("parent");
        bool status = false;
        auto parent = static_cast<BciNumber>(childElement.text().toUInt(&status));
        if (!status)
            qWarning() << name << className;
        delegate::Item item(parentType, itemType, parent, widgetGroup);
        return item;
    }
    default:
        return delegate::Item(parentType, itemType, BciNumber::dummyElement, widgetGroup);
    }
}

void XmlParser::traverseNode(const QDomNode &node, ModuleSettings *const settings, GlobalSettings &gsettings)
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
                    if (!isCorrectModule(domElement.attribute("mtypem", ""), domElement.attribute("mtypeb", "")))
                    {
                        domNode = domNode.nextSibling();
                        continue;
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
                    if (Board::GetInstance().interfaceType() == Board::RS485)
                        settings->ifaceSettings = (BaseInterface::iface()->parseSettings(domElement));

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "protocom")
                {
                    if (Board::GetInstance().interfaceType() == Board::USB
                        || Board::GetInstance().interfaceType() == Board::Emulator)
                        settings->ifaceSettings = (BaseInterface::iface()->parseSettings(domElement));

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "iec60870")
                {
                    if (Board::GetInstance().interfaceType() == Board::Ethernet)
                        settings->ifaceSettings = (BaseInterface::iface()->parseSettings(domElement));

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "record")
                {

                    settings->configSettings.push_back(parseRecord(domElement, gsettings.s2widgetMap));
                    domNode = domNode.nextSibling();
                    continue;
                }
            }
        }
        traverseNode(domNode, settings, gsettings);
        domNode = domNode.nextSibling();
    }
}

void XmlParser::traverseNode(const QDomNode &node, GlobalSettings &settings)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == "record")
                {
#ifdef XML_DEBUG
                    qDebug() << domElement.text();
#endif
                    domElement = domElement.firstChild().toElement();
                    BciNumber id = static_cast<BciNumber>(XmlParser::parseInt32(domElement));
                    domElement = domElement.nextSibling().toElement();
                    settings.s2filesMap->insert({ id, parseType(domElement) });
                    domElement = domElement.nextSibling().toElement();
                    settings.s2widgetMap->insert({ id, parseWidget(domElement) });

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "map")
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
                        if (domElement.tagName() == "key")
                        {
                            key = QVariant(domElement.text()).value<Key>();
                        }
                        else if (domElement.tagName() == "value")
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
