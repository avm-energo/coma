#include "xml/xmleditor/models/xmlhidedatamodel.h"

#include <xml/xmltags.h>

constexpr int SGroupDataRole = 0x0106;   ///< Role for hiding data SGroupHideData.
constexpr int S2RecordDataRole = 0x0107; /// < Role for hiding data S2RecordHideData.

/// \brief Structure, that stores a hiding data for 'sgroup' node.
struct SGroupHideData
{
    int count = 1;
    QString tooltip = "", view = "float";
    QStringList array = {};
};
Q_DECLARE_METATYPE(SGroupHideData);

/// \brief Structure, that stores a hiding data for 'record' node from 's2files.xml'.
/// \details Contains data of 'widget' node.
struct S2RecordHideData
{
    int min = 0, max = 0, decimals = 0, group = 0, count = 0;
    bool isEnabled = false;
    QString type = "", string = "", tooltip = "", field = "";
    QStringList array = {};
};
Q_DECLARE_METATYPE(S2RecordHideData);

XmlHideDataModel::XmlHideDataModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

QStringList XmlHideDataModel::getRowData(const int row)
{
    // Собираем открытые данные
    QStringList retList = BaseEditorModel::getRowData(row);
    // Собираем скрытые данные
    if (row >= 0 && row < rowCount())
    {
        switch (m_type)
        {
        case ModelType::SGroup:
        {
            auto hiding = data(index(row, 0), SGroupDataRole);
            if (hiding.isValid() && hiding.canConvert<SGroupHideData>())
                retList.append(convertFromSGroupData(hiding.value<SGroupHideData>()));
            break;
        }
        default:
            break;
        }
    }
    return retList;
}

void XmlHideDataModel::create(const QStringList &saved, int *row)
{
    switch (m_type)
    {
    case ModelType::SGroup:
        Q_ASSERT(saved.count() == 6);
        BaseEditorModel::create({ saved[0], saved[1] }, row);
        if (*row >= 0 && *row < rowCount())
        {
            auto newHide = convertToSGroupData({ saved[2], saved[3], saved[5], saved[4] });
            setData(index(*row, 0), QVariant::fromValue(newHide), SGroupDataRole);
        }
        emit modelChanged();
        break;
    default:
        break;
    }
}

void XmlHideDataModel::update(const QStringList &saved, const int row)
{
    switch (m_type)
    {
    case ModelType::SGroup:
        BaseEditorModel::update({ saved[0], saved[1] }, row);
        if (row >= 0 && row < rowCount())
        {
            auto newHide = convertToSGroupData({ saved[2], saved[3], saved[5], saved[4] });
            setData(index(row, 0), QVariant::fromValue(newHide), SGroupDataRole);
        }
        emit modelChanged();
        break;
    default:
        break;
    }
}

void XmlHideDataModel::parseNode(QDomNode &node, int &row)
{
    switch (m_type)
    {
    case ModelType::SGroup:
        parseTag(node, tags::start_addr, row, 0, "", true); // Адрес
        parseAttribute(node, tags::desc, row, 1);           // Заголовок
        setData(index(row, 0), QVariant::fromValue(parseSGroupData(node)), SGroupDataRole);
        break;
    case ModelType::S2Records:
        parseTag(node, tags::id, row, 0, "", true);  // ID
        parseTag(node, tags::name, row, 1);          // Имя записи
        parseTag(node, tags::type, row, 2, "DWORD"); // Тип данных записи
        parseTag(node, tags::description, row, 3);   // Описание
        setData(index(row, 0), QVariant::fromValue(parseS2RecordData(node)), S2RecordDataRole);
        break;
    default:
        break;
    }
}

QDomElement XmlHideDataModel::toNode(QDomDocument &doc)
{
    switch (m_type)
    {
    case ModelType::SGroup:
    {
        auto sgroupNode = makeElement(doc, tags::sgroup);
        fillSGroupNode(doc, sgroupNode);
        return sgroupNode;
    }
    case ModelType::S2Records:
    {
        auto s2recNode = makeElement(doc, tags::records);
        return s2recNode;
    }
    default:
        return makeElement(doc, "undefined");
    }
}

void XmlHideDataModel::parseInteger(const QDomNode &source, const QString &nodeName, int &dest)
{
    auto state = false;
    auto search = source.firstChildElement(nodeName);
    if (!search.isNull())
    {
        auto intText = search.firstChild().toText().data();
        auto value = intText.toInt(&state);
        if (state)
            dest = value;
    }
}

void XmlHideDataModel::parseText(const QDomNode &source, const QString &nodeName, QString &dest)
{
    auto search = source.firstChildElement(nodeName);
    if (!search.isNull())
        dest = search.firstChild().toText().data();
}

void XmlHideDataModel::parseStringArray(const QDomNode &source, QStringList &dest)
{
    auto strArrayNode = source.firstChildElement(tags::str_array);
    if (!strArrayNode.isNull())
    {
        auto strItemNode = strArrayNode.firstChild();
        while (!strItemNode.isNull())
        {
            if (!strItemNode.isComment())
            {
                auto strItem = strItemNode.firstChild().toText().data();
                dest.append(strItem);
            }
            strItemNode = strItemNode.nextSibling();
        }
    }
}

SGroupHideData XmlHideDataModel::parseSGroupData(QDomNode &node)
{
    SGroupHideData retVal;
    parseInteger(node, tags::count, retVal.count);                 // Парсим тег count
    parseText(node, tags::tooltip, retVal.tooltip);                // Парсим тег toolTip
    retVal.view = node.toElement().attribute(tags::view, "float"); // Парсим аттрибут view
    parseStringArray(node, retVal.array);                          // Парсим тег string-array
    return retVal;
}

SGroupHideData XmlHideDataModel::convertToSGroupData(const QStringList &input)
{
    SGroupHideData hiding;
    auto state = false;
    auto count = input[0].toInt(&state);
    if (state)
        hiding.count = count;
    hiding.tooltip = input[1];
    hiding.view = input[2];
    if (!input[3].isEmpty())
        hiding.array = input[3].split(',');
    return hiding;
}

QStringList XmlHideDataModel::convertFromSGroupData(const SGroupHideData &input)
{
    QStringList retList;
    retList.append(QString::number(input.count));
    retList.append(input.tooltip);
    retList.append(input.array.join(','));
    retList.append(input.view);
    return retList;
}

void XmlHideDataModel::fillSGroupNode(QDomDocument &doc, QDomElement &sgroupNode)
{
    for (auto row = 0; row < rowCount(); row++)
    {
        // Обходим элемент для возвращения назад стороной
        if (data(index(row, 0)).value<QString>() == "..")
            continue;

        // Видимые данные
        auto mwidget = makeElement(doc, tags::mwidget);
        makeElement(doc, mwidget, tags::start_addr, data(index(row, 0)));
        setAttribute(doc, mwidget, tags::desc, data(index(row, 1)));
        // Скрытые данные
        auto hideDataVar = data(index(row, 0), SGroupDataRole);
        if (hideDataVar.isValid() && hideDataVar.canConvert<SGroupHideData>())
        {
            auto hideData = hideDataVar.value<SGroupHideData>();
            if (hideData.count != 1)
                makeElement(doc, mwidget, tags::count, QString::number(hideData.count));

            if (!hideData.tooltip.isEmpty())
                makeElement(doc, mwidget, tags::tooltip, hideData.tooltip);

            if (!hideData.view.isEmpty())
                setAttribute(doc, mwidget, tags::view, hideData.view);

            if (!(hideData.array.isEmpty() || (hideData.array.size() == 1 && hideData.array.first().isEmpty())))
            {
                auto strArray = makeElement(doc, tags::str_array);
                for (const auto &str : qAsConst(hideData.array))
                    makeElement(doc, strArray, tags::item, str);
                mwidget.appendChild(strArray);
            }
        }
        sgroupNode.appendChild(mwidget);
    }
}

S2RecordHideData XmlHideDataModel::parseS2RecordData(QDomNode &node)
{
    S2RecordHideData retVal;
    auto widgetNode = node.firstChildElement(tags::widget);
    if (!widgetNode.isNull())
    {
        retVal.isEnabled = true;
        parseInteger(widgetNode, tags::group, retVal.group);  // Парсим тег group
        parseInteger(widgetNode, tags::count, retVal.count);  // Парсим тег count
        parseInteger(widgetNode, tags::min, retVal.min);      // Парсим тег min
        parseInteger(widgetNode, tags::max, retVal.max);      // Парсим тег max
        parseText(widgetNode, tags::type, retVal.type);       // Парсим тег type
        parseText(widgetNode, tags::string, retVal.string);   // Парсим тег string
        parseText(widgetNode, tags::tooltip, retVal.tooltip); // Парсим тег tooltip
        parseText(widgetNode, tags::field, retVal.field);     // Парсим тег field
        parseStringArray(widgetNode, retVal.array);           // Парсим тег string-array
    }
    return retVal;
}
