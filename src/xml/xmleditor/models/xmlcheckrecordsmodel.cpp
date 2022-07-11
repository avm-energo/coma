#include "xmlcheckmodel.h"

XmlCheckRecordsModel::XmlCheckRecordsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlCheckRecordsModel::parseNode(QDomNode &node, int &row)
{
    if (node.hasAttributes())
    {
        // Заголовок
        auto headerNode = node.attributes().namedItem("header");
        if (!headerNode.isNull())
        {
            auto headerIndex = index(row, 0);
            auto header = headerNode.firstChild().toText().data();
            setData(headerIndex, header);
        }
        // Описание
        auto descNode = node.attributes().namedItem("desc");
        if (!descNode.isNull())
        {
            auto descIndex = index(row, 1);
            auto desc = descNode.firstChild().toText().data();
            setData(descIndex, desc);
        }
    }
    // Группа
    auto groupNode = node.firstChildElement("group");
    if (!groupNode.isNull())
    {
        auto groupIndex = index(row, 2);
        auto group = groupNode.firstChild().toText().data();
        setData(groupIndex, group);
    }
    // Стартовый адрес
    auto addrNode = node.firstChildElement("start-addr");
    if (!addrNode.isNull())
    {
        auto addrIndex = index(row, 3);
        auto addr = addrNode.firstChild().toText().data();
        setData(addrIndex, addr);
    }
    // Количество
    auto countNode = node.firstChildElement("count");
    if (!countNode.isNull())
    {
        auto countIndex = index(row, 4);
        auto count = countNode.firstChild().toText().data();
        setData(countIndex, count);
    }
}
