#include "xmlmainmodel.h"

XmlMainModel::XmlMainModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlMainModel::parseNode(QDomNode &node, int &row)
{
    Q_UNUSED(node);
    Q_UNUSED(row);
}
