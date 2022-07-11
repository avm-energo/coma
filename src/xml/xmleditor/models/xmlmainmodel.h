#ifndef XMLMAINMODEL_H
#define XMLMAINMODEL_H

#include "xmlmodel.h"

class XmlMainModel : public XmlModel
{
    Q_OBJECT
public:
    XmlMainModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

#endif // XMLMAINMODEL_H
