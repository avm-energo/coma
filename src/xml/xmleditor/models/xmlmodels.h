#ifndef XMLMODELS_H
#define XMLMODELS_H

#include "xmlmodel.h"

class XmlBaseModel : public XmlModel
{
public:
    explicit XmlBaseModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// SIGNALS

class XmlSignalsModel : public XmlModel
{
public:
    explicit XmlSignalsModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// SECTIONS

class XmlSectionTabsModel : public XmlModel
{
public:
    explicit XmlSectionTabsModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlSectionsModel : public XmlModel
{
public:
    explicit XmlSectionsModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlSectionModel : public XmlModel
{
public:
    explicit XmlSectionModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlSectionGroupsModel : public XmlModel
{
public:
    explicit XmlSectionGroupsModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// ALARMS

class XmlAlarmsModel : public XmlModel
{
public:
    explicit XmlAlarmsModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// JOURNALS

class XmlWorkJoursModel : public XmlModel
{
public:
    explicit XmlWorkJoursModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlMeasJoursModel : public XmlModel
{
public:
    explicit XmlMeasJoursModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// DATA EXCHANGE PROTOCOLS

class XmlModbusModel : public XmlModel
{
public:
    explicit XmlModbusModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlProtocomModel : public XmlModel
{
public:
    explicit XmlProtocomModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlIECModel : public XmlModel
{
public:
    explicit XmlIECModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// S2 CONFIG

class XmlConfigModel : public XmlModel
{
public:
    explicit XmlConfigModel(int rows = 1, int cols = 1, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

#endif // XMLMODELS_H
