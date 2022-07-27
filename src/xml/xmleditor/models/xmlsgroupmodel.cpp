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

bool XmlSGroupModel::remove(int row)
{
    auto state = XmlModel::remove(row);
    if (state)
        hideData.remove(row);
    return state;
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

QString accumulateStr(QStringList &src)
{
    QString result = "";
    if (!src.isEmpty())
    {
        auto last = src.last();
        for (auto &str : src)
        {
            result += str;
            if (str != last)
                result += ',';
        }
    }
    return result;
}

QStringList deaccumulateStr(const QString &src)
{
    QStringList result = {};
    if (src.length() > 0)
    {
        auto copy = src;
        while (copy.length() > 0)
        {
            auto index = copy.indexOf(',');
            auto item = copy.left(index);
            result.append(item);
            if (index == -1)
                break;
            copy = copy.right(copy.length() - (index + 1));
        }
    }
    return result;
}

SGroupHideData convertHideData(QStringList &input)
{
    SGroupHideData hiding;
    auto state = false;
    auto count = input[0].toInt(&state);
    if (state)
        hiding.count = count;
    hiding.tooltip = input[1];
    hiding.array = deaccumulateStr(input[2]);
    return hiding;
}
