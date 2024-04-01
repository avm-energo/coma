#include "xml/xmleditor/models/xmlhidedatamodel.h"

#include <xml/xmltags.h>

XmlHideDataModel::XmlHideDataModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

QStringList XmlHideDataModel::getRowData(const int row)
{
    QStringList retList;
    if (row >= 0 && row < rowCount())
    {
        auto cols = columnCount();
        retList.reserve(cols);
        // Собираем открытые данные
        for (auto column = 0; column < cols; column++)
        {
            auto itemIndex = index(row, column);
            auto item = data(itemIndex);
            if (item.isValid() && item.canConvert<QString>())
            {
                auto itemStr = item.value<QString>();
                retList.append(itemStr);
            }
            else
                retList.append("");
        }
        // Собираем скрытые данные
        auto hiding = data(index(row, 0), SGroupDataRole);
        if (hiding.isValid() && hiding.canConvert<SGroupHideData>())
        {
            auto hidingVal = hiding.value<SGroupHideData>();
            retList.append(QString::number(hidingVal.count));
            retList.append(hidingVal.tooltip);
            retList.append(hidingVal.array.join(','));
            retList.append(hidingVal.view);
        }
    }
    return retList;
}

void XmlHideDataModel::create(const QStringList &saved, int *row)
{
    Q_ASSERT(saved.count() == 6);
    BaseEditorModel::create({ saved[0], saved[1] }, row);
    if (*row >= 0 && *row < rowCount())
    {
        auto newHide = convertHideData({ saved[2], saved[3], saved[5], saved[4] });
        setData(index(*row, 0), QVariant::fromValue(newHide), SGroupDataRole);
    }
    emit modelChanged();
}

void XmlHideDataModel::update(const QStringList &saved, const int row)
{
    BaseEditorModel::update({ saved[0], saved[1] }, row);
    if (row >= 0 && row < rowCount())
    {
        auto newHide = convertHideData({ saved[2], saved[3], saved[5], saved[4] });
        setData(index(row, 0), QVariant::fromValue(newHide), SGroupDataRole);
    }
    emit modelChanged();
}

void XmlHideDataModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, tags::start_addr, row, 0); // Адрес
    parseAttribute(node, tags::desc, row, 1); // Заголовок
    setData(index(row, 0), QVariant::fromValue(parseHideData(node)), SGroupDataRole);
}

QDomElement XmlHideDataModel::toNode(QDomDocument &doc)
{
    auto sgroupNode = makeElement(doc, tags::sgroup);
    for (auto row = 0; row < rowCount(); row++)
    {
        // TODO: костыль
        if (data(index(row, 0)).value<QString>() != "..")
        {
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
    return sgroupNode;
}

SGroupHideData XmlHideDataModel::parseHideData(QDomNode &node)
{
    SGroupHideData retVal;
    auto state = false;
    // Парсим тег count
    auto countNode = node.firstChildElement(tags::count);
    if (!countNode.isNull())
    {
        auto countText = countNode.firstChild().toText().data();
        auto count = countText.toInt(&state);
        if (state)
            retVal.count = count;
    }
    // Парсим тег toolTip
    auto tooltipNode = node.firstChildElement(tags::tooltip);
    if (!tooltipNode.isNull())
    {
        auto tooltip = tooltipNode.firstChild().toText().data();
        retVal.tooltip = tooltip;
    }
    // Парсим аттрибут view
    retVal.view = node.toElement().attribute(tags::view, "float");
    // Парсим тег string-array
    auto strArrayNode = node.firstChildElement(tags::str_array);
    if (!strArrayNode.isNull())
    {
        auto strItemNode = strArrayNode.firstChild();
        while (!strItemNode.isNull())
        {
            if (!strItemNode.isComment())
            {
                auto strItem = strItemNode.firstChild().toText().data();
                retVal.array.append(strItem);
            }
            strItemNode = strItemNode.nextSibling();
        }
    }
    return retVal;
}

SGroupHideData XmlHideDataModel::convertHideData(const QStringList &input)
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
