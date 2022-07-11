#include "xmlcheckmodel.h"

XmlCheckGroupsModel::XmlCheckGroupsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlCheckGroupsModel::parseNode(QDomNode &node, int &row)
{
    // Имя группы
    auto nameNode = node.firstChildElement("name");
    if (!nameNode.isNull())
    {
        auto nameIndex = index(row, 0);
        auto name = nameNode.firstChild().toText().data();
        setData(nameIndex, name);
    }
    // Номер группы
    auto idNode = node.firstChildElement("id");
    if (!idNode.isNull())
    {
        auto idIndex = index(row, 1);
        auto id = idNode.firstChild().toText().data();
        setData(idIndex, id);
    }
}
