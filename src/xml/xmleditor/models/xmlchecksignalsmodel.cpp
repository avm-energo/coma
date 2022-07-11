#include "xmlcheckmodel.h"

XmlCheckSignalsModel::XmlCheckSignalsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlCheckSignalsModel::parseNode(QDomNode &node, int &row)
{
    // Тип сигнала
    auto typeNode = node.firstChildElement("type");
    if (!typeNode.isNull())
    {
        auto typeIndex = index(row, 0);
        auto type = typeNode.firstChild().toText().data();
        setData(typeIndex, type);
    }
    // Стартовый адрес
    auto addrNode = node.firstChildElement("start-addr");
    if (!addrNode.isNull())
    {
        auto addrIndex = index(row, 1);
        auto addr = addrNode.firstChild().toText().data();
        setData(addrIndex, addr);
    }
    // Количество
    auto countNode = node.firstChildElement("count");
    if (!countNode.isNull())
    {
        auto countIndex = index(row, 2);
        auto count = countNode.firstChild().toText().data();
        setData(countIndex, count);
    }
}
