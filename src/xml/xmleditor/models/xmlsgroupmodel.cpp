#include "xmlsgroupmodel.h"

XmlSGroupModel::XmlSGroupModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent)
{
}

QVariant XmlSGroupModel::data(const QModelIndex &index, int nRole) const
{
    if (index.isValid() && nRole == SGroupDataRole)
    {
        return hideData.value(index.row(), QVariant());
    }
    else
        return XmlModel::data(index, nRole);
}

bool XmlSGroupModel::setData(const QModelIndex &index, const QVariant &val, int nRole)
{
    if (index.isValid() && nRole == SGroupDataRole)
    {
        auto state = false;
        if (val.isValid() && val.canConvert<SGroupHideData>())
        {
            hideData.insert(index.row(), val);
            state = true;
        }
        return state;
    }
    else
        return XmlModel::setData(index, val, nRole);
}

void XmlSGroupModel::parseNode(QDomNode &node, int &row)
{
    parseAttribute(node, "desc", row, 0); // Заголовок
    parseTag(node, "start-addr", row, 1); // Адрес
    setData(index(row, 0), QVariant::fromValue(parseHideData(node)), SGroupDataRole);
}

SGroupHideData XmlSGroupModel::parseHideData(QDomNode &node)
{
    SGroupHideData retVal;
    auto state = false;
    // Парсим тег count
    auto countNode = node.firstChildElement("count");
    if (!countNode.isNull())
    {
        auto countText = countNode.firstChild().toText().data();
        auto count = countText.toInt(&state);
        if (state)
            retVal.count = count;
    }
    // Парсим тег toolTip
    auto tooltipNode = node.firstChildElement("toolTip");
    if (!tooltipNode.isNull())
    {
        auto tooltip = tooltipNode.firstChild().toText().data();
        retVal.tooltip = tooltip;
    }
    // Парсим тег string-array
    auto strArrayNode = node.firstChildElement("string-array");
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
