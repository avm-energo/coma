#include "xmlparser.h"

#include "../module/module.h"
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

// using BYTE = unsigned char;
// using WORD = unsigned short;
// using DWORD = unsigned int;
// using BYTE_4t = std::array<BYTE, 4>;
// using WORD_4t = std::array<WORD, 4>;
// using BYTE_16t = std::array<BYTE, 16>;
// using WORD_16t = std::array<WORD, 16>;
// using FLOAT_2t = std::array<float, 2>;
// using FLOAT_2t_2t = std::array<FLOAT_2t, 2>;
// using FLOAT_3t = std::array<float, 3>;
// using FLOAT_6t = std::array<float, 6>;
// using FLOAT_8t = std::array<float, 8>;

// ctti::unnamed_type_id<WORD>().hash()
ctti::unnamed_type_id_t XmlParser::parseType(QDomElement domElement)
{
    using namespace DataTypes;
    auto name = domElement.text();
    name.replace(" ", "");
    const auto arrSize = name.count('[');
    switch (arrSize)
    {
    case 0:
        if (name.contains("BYTE", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<BYTE>().hash();
        if (name.contains("DWORD", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DWORD>().hash();
        if (name.contains("WORD", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD>().hash();
        if (name.contains("float", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<float>().hash();

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
                    //      qDebug() << "Attr: " << domElement.attribute("name", "")
                    //               << "\tValue: " << /*qPrintable*/ (domElement.text());
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

                    // qDebug() << "Attr: " << domElement.attribute("mtypea", "") << domElement.attribute("mtypeb", "");
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
                //                else
                //                {
                //                    qDebug() << "TagName: " << domElement.tagName() << "\tText: " << /*qPrintable*/
                //                    (domElement.text());
                //                }
            }
        }
        traverseNode(domNode, settings);
        domNode = domNode.nextSibling();
    }
}

void XmlParser::traverseNode(const QDomNode &node, S2DataTypes::valueTypeMap *const settings)
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

                    domElement = domElement.firstChild().toElement();
                    const auto id = XmlParser::parseInt32(domElement);
                    domElement = domElement.nextSibling().toElement();
                    settings->insert({ id, parseType(domElement) });
#ifdef XML_DEBUG
                    const auto type = domElement.text();
                    qDebug() << id << type;
#endif
                    domNode = domNode.nextSibling();
                    continue;
                }
            }
        }
        traverseNode(domNode, settings);
        domNode = domNode.nextSibling();
    }
}
