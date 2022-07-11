#include "xmlcheckmodel.h"

XmlCheckModel::XmlCheckModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlCheckModel::parseNode(QDomNode &node, int &row)
{
    Q_UNUSED(node);
    Q_UNUSED(row);
}
