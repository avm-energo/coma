#include "xmlparser.h"

#include "../module/module.h"
#include "../widgets/checkboxgroup.h"
#include "board.h"
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
    alarm.name = domElement.attribute("name", "");
    auto element = domElement.firstChildElement("string-array");
    alarm.desc = parseStringList(element);
    element = domElement.firstChildElement("color");
    alarm.color = element.isNull() ? "" : element.text();
    element = domElement.firstChildElement("quint32");
    while (!element.isNull())
    {
        const auto name = element.attribute("name", "");
        if (name.contains("flags", Qt::CaseInsensitive))
            alarm.flags = parseHexInt32(element);
        if (name.contains("addr", Qt::CaseInsensitive))
            alarm.startAddr = parseInt32(element);
        element = element.nextSiblingElement("quint32");
    }

    return alarm;
}

DataTypes::Journal XmlParser::parseJournal(QDomElement domElement)
{
    DataTypes::Journal journal;
#ifdef XML_DEBUG
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
#endif
    journal.name = domElement.attribute("name", "");
    journal.id = parseInt32(domElement.firstChildElement("quint32"));
    domElement = domElement.firstChildElement("string-array");
    while (!domElement.isNull())
    {
        const auto name = domElement.attribute("name", "");
        if (name.contains("description"), Qt::CaseInsensitive)
            journal.desc = parseStringList(domElement);
        if (name.contains("header"), Qt::CaseInsensitive)
            journal.header = parseStringList(domElement);
        domElement = domElement.nextSiblingElement("string-array");
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

quint64 XmlParser::parseHexInt32(QDomElement domElement)
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
        if (name.contains("WORD[4]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD_4t>().hash();
        if (name.contains("WORD[8]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD_8t>().hash();
        if (name.contains("WORD[16]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD_16t>().hash();
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
    QString className = domElement.attribute("class");
    auto type = parseType(domElement.firstChildElement("type"));

    if (!className.isEmpty())
    {
        return parseItem(domElement, type);
    }
    QStringList items;

    QDomElement childElement = domElement.firstChildElement("string-array");
    if (!childElement.isNull())
        items = parseStringList(childElement);

    childElement = domElement.firstChildElement("group");
    auto widgetGroup = static_cast<delegate::WidgetGroup>(childElement.text().toInt());

    const QString description = domElement.firstChildElement("string").text();
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
        widget.items = items;
        return widget;
    }
    case ctti::unnamed_type_id<QComboBox>().hash():
    {
        delegate::QComboBox widget(type, widgetGroup);

        widget.desc = description;
        widget.items = items;
        QDomElement childElement = domElement.firstChildElement("field");
        // QComboBox depends on index by default
        widget.primaryField
            = childElement.text().contains("data") ? delegate::QComboBox::data : delegate::QComboBox::index;
        return widget;
    }

    default:
    {
    }
    }
    return delegate::Widget(type, description, widgetGroup);
}

delegate::Item XmlParser::parseItem(QDomElement domElement, ctti::unnamed_type_id_t parentType)
{
    auto name = domElement.text();
#ifdef XML_DEBUG
    qDebug() << name;
#endif
    QString className = domElement.attribute("class");
    if (className.isEmpty())
        return { 0 };
    auto classes = QMetaEnum::fromType<delegate::ItemType>();
    bool status = false;
    auto itemType = static_cast<delegate::ItemType>(classes.keyToValue(className.toStdString().c_str(), &status));
    if (!status)
        return { 0 };

    QDomElement childElement = domElement.firstChildElement("group");
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

void XmlParser::traverseNode(const QDomNode &node, ModuleSettings *const settings)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == "quint32")
                {
                    XmlParser::parseInt32(domElement);
                }
                if (domElement.tagName() == "string-array")
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

                    qDebug() << "Attr: " << domElement.attribute("name", "");
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

                if (domElement.tagName() == "modbus")
                {
                    if (Board::GetInstance().interfaceType() == Board::RS485)
                        settings->ifaceSettings = (BaseInterface::iface()->parseSettings(domElement));

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "protocom")
                {
                    if (Board::GetInstance().interfaceType() == Board::USB)
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
            }
        }
        traverseNode(domNode, settings);
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
            }
        }
        traverseNode(domNode, settings);
        domNode = domNode.nextSibling();
    }
}
