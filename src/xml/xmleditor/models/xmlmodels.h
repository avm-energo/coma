#ifndef XMLMODELS_H
#define XMLMODELS_H

#include "xmlmodel.h"

class XmlMainModel : public XmlModel
{
public:
    explicit XmlMainModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlCheckModel : public XmlModel
{
public:
    explicit XmlCheckModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlCheckGroupsModel : public XmlModel
{
public:
    explicit XmlCheckGroupsModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlCheckSignalsModel : public XmlModel
{
public:
    explicit XmlCheckSignalsModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlCheckRecordsModel : public XmlModel
{
public:
    explicit XmlCheckRecordsModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

#endif // XMLMODELS_H
