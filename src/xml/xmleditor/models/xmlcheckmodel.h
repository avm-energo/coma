#ifndef XMLCHECKMODEL_H
#define XMLCHECKMODEL_H

#include "xmlmodel.h"

class XmlCheckModel : public XmlModel
{
public:
    XmlCheckModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void setDataNode(GroupTypes type, QDomNode &root) override;
};

class XmlCheckGroupsModel : public XmlModel
{
public:
    XmlCheckGroupsModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void setDataNode(GroupTypes type, QDomNode &root) override;
};

class XmlCheckSignalsModel : public XmlModel
{
public:
    XmlCheckSignalsModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void setDataNode(GroupTypes type, QDomNode &root) override;
};

class XmlCheckRecordsModel : public XmlModel
{
public:
    XmlCheckRecordsModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void setDataNode(GroupTypes type, QDomNode &root) override;
};

#endif // XMLCHECKMODEL_H
