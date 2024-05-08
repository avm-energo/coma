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
    int min = 0, max = 0, decimals = 0, group = 0, count = 1, parent = 0;
    bool isEnabled = false;
    QString classname = "", type = "", string = "", tooltip = "", field = "";
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
        case ModelType::S2Records:
        {
            auto hiding = data(index(row, 0), S2RecordDataRole);
            if (hiding.isValid() && hiding.canConvert<S2RecordHideData>())
                retList.append(convertFromS2RecordData(hiding.value<S2RecordHideData>()));
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
        BaseEditorModel::create(saved.mid(0, 2), row);
        if (*row >= 0 && *row < rowCount())
        {
            auto value = convertToSGroupData(saved.mid(2, 4));
            setData(index(*row, 0), QVariant::fromValue(value), SGroupDataRole);
        }
        emit modelChanged();
        break;
    case ModelType::S2Records:
        Q_ASSERT(saved.count() == 17);
        BaseEditorModel::create(saved.mid(0, 4), row);
        if (*row >= 0 && *row < rowCount())
        {
            auto value = convertToS2RecordData(saved.mid(4));
            setData(index(*row, 0), QVariant::fromValue(value), S2RecordDataRole);
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
        Q_ASSERT(saved.count() == 6);
        BaseEditorModel::update(saved.mid(0, 2), row);
        if (row >= 0 && row < rowCount())
        {
            auto newHide = convertToSGroupData(saved.mid(2, 4));
            setData(index(row, 0), QVariant::fromValue(newHide), SGroupDataRole);
        }
        emit modelChanged();
        break;
    case ModelType::S2Records:
        Q_ASSERT(saved.count() == 17);
        BaseEditorModel::update(saved.mid(0, 4), row);
        if (row >= 0 && row < rowCount())
        {
            auto value = convertToS2RecordData(saved.mid(4));
            setData(index(row, 0), QVariant::fromValue(value), S2RecordDataRole);
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
        return makeSGroupNode(doc);
    case ModelType::S2Records:
        return makeS2RecordsNode(doc);
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
    Q_ASSERT(input.count() == 4);
    SGroupHideData hiding;
    auto state = false;
    auto count = input[0].toInt(&state);
    if (state)
        hiding.count = count;
    hiding.tooltip = input[1];
    if (!input[2].isEmpty())
        hiding.array = input[2].split(',');
    hiding.view = input[3];
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

QDomElement XmlHideDataModel::makeSGroupNode(QDomDocument &doc)
{
    auto sgroupNode = makeElement(doc, tags::sgroup);
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
    return sgroupNode;
}

S2RecordHideData XmlHideDataModel::parseS2RecordData(QDomNode &node)
{
    S2RecordHideData retVal;
    auto widgetNode = node.firstChildElement(tags::widget);
    if (!widgetNode.isNull())
    {
        retVal.isEnabled = true;
        retVal.classname                                          // Парсим аттрибут class
            = widgetNode.toElement().attribute(tags::class_, ""); //
        parseInteger(widgetNode, tags::group, retVal.group);      // Парсим тег group
        parseInteger(widgetNode, tags::count, retVal.count);      // Парсим тег count
        parseInteger(widgetNode, tags::min, retVal.min);          // Парсим тег min
        parseInteger(widgetNode, tags::max, retVal.max);          // Парсим тег max
        parseInteger(widgetNode, tags::parent, retVal.parent);    // Парсим тег parent
        parseText(widgetNode, tags::type, retVal.type);           // Парсим тег type
        parseText(widgetNode, tags::string, retVal.string);       // Парсим тег string
        parseText(widgetNode, tags::tooltip, retVal.tooltip);     // Парсим тег tooltip
        parseText(widgetNode, tags::field, retVal.field);         // Парсим тег field
        parseStringArray(widgetNode, retVal.array);               // Парсим тег string-array
    }
    return retVal;
}

QDomElement XmlHideDataModel::makeS2RecordsNode(QDomDocument &doc)
{
    auto s2recordsNode = makeElement(doc, tags::records);
    for (auto row = 0; row < rowCount(); row++)
    {
        // Обходим элемент для возвращения назад стороной
        if (data(index(row, 0)).value<QString>() == "..")
            continue;

        // Видимые данные
        auto record = makeElement(doc, tags::record);
        makeElement(doc, record, tags::id, data(index(row, 0)));
        makeElement(doc, record, tags::name, data(index(row, 1)));
        makeElement(doc, record, tags::type, data(index(row, 2)));
        makeElement(doc, record, tags::description, data(index(row, 3)));
        // Скрытые данные
        auto hideData = data(index(row, 0), S2RecordDataRole);
        if (hideData.isValid() && hideData.canConvert<S2RecordHideData>())
        {
            auto s2recordData = hideData.value<S2RecordHideData>();
            if (s2recordData.isEnabled && !s2recordData.type.isEmpty())
            {
                auto widget = makeElement(doc, tags::widget);
                if (!s2recordData.classname.isEmpty())
                    setAttribute(doc, widget, tags::class_, s2recordData.classname);
                makeElement(doc, widget, tags::type, s2recordData.type);
                makeElement(doc, widget, tags::group, QString::number(s2recordData.group));
                if (!s2recordData.string.isEmpty())
                    makeElement(doc, widget, tags::string, s2recordData.string);
                if (!s2recordData.tooltip.isEmpty())
                    makeElement(doc, widget, tags::tooltip, s2recordData.tooltip);
                if (s2recordData.classname.contains("ModbusItem", Qt::CaseInsensitive) && s2recordData.parent > 0)
                    makeElement(doc, widget, tags::parent, QString::number(s2recordData.parent));
                if (s2recordData.count != 1)
                    makeElement(doc, widget, tags::count, QString::number(s2recordData.count));
                if (s2recordData.type.contains("DoubleSpinBoxGroup", Qt::CaseInsensitive) || //
                    s2recordData.type.contains("QDoubleSpinBox", Qt::CaseInsensitive))
                {
                    makeElement(doc, widget, tags::min, QString::number(s2recordData.min));
                    makeElement(doc, widget, tags::max, QString::number(s2recordData.max));
                    makeElement(doc, widget, tags::decimals, QString::number(s2recordData.decimals));
                }
                if (s2recordData.type.contains("QComboBox", Qt::CaseInsensitive) && !s2recordData.field.isEmpty())
                    makeElement(doc, widget, tags::field, s2recordData.field);
                if (!(s2recordData.array.isEmpty()
                        || (s2recordData.array.size() == 1 && s2recordData.array.first().isEmpty())))
                {
                    auto strArray = makeElement(doc, tags::str_array);
                    for (const auto &str : qAsConst(s2recordData.array))
                        makeElement(doc, strArray, tags::item, str);
                    widget.appendChild(strArray);
                }
                record.appendChild(widget);
            }
        }
        s2recordsNode.appendChild(record);
    }
    return s2recordsNode;
}

QStringList XmlHideDataModel::convertFromS2RecordData(const S2RecordHideData &input)
{
    QStringList retList;
    retList.append(input.isEnabled ? "1" : "0");
    retList.append(input.classname);
    retList.append(input.type);
    retList.append(QString::number(input.group));
    retList.append(input.string);
    retList.append(input.tooltip);
    retList.append(QString::number(input.parent));
    retList.append(QString::number(input.count));
    retList.append(QString::number(input.min));
    retList.append(QString::number(input.max));
    retList.append(QString::number(input.decimals));
    retList.append(input.field);
    retList.append(input.array.join(','));
    return retList;
}

S2RecordHideData XmlHideDataModel::convertToS2RecordData(const QStringList &input)
{
    Q_ASSERT(input.count() == 13);
    S2RecordHideData retVal;
    retVal.isEnabled = (input[0] == "1");
    retVal.classname = input[1];
    retVal.type = input[2];
    bool state = false;
    auto integer = input[3].toInt(&state);
    if (state)
        retVal.group = integer;
    retVal.string = input[4];
    retVal.tooltip = input[5];
    integer = input[6].toInt(&state);
    if (state)
        retVal.parent = integer;
    integer = input[7].toInt(&state);
    if (state)
        retVal.count = integer;
    integer = input[8].toInt(&state);
    if (state)
        retVal.min = integer;
    integer = input[9].toInt(&state);
    if (state)
        retVal.max = integer;
    integer = input[10].toInt(&state);
    if (state)
        retVal.decimals = integer;
    retVal.field = input[11];
    if (!input[12].isEmpty())
        retVal.array = input[12].split(',');
    return retVal;
}
