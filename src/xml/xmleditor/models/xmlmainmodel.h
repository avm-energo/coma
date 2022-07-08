#ifndef XMLMAINMODEL_H
#define XMLMAINMODEL_H

#include "xmlmodel.h"

class XmlMainModel : public XmlModel
{
    Q_OBJECT
private:
    void CheckChilds(QDomNode &child, int &row);

public:
    XmlMainModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void setDataNode(GroupTypes type, QDomNode &root) override;
};

#endif // XMLMAINMODEL_H
