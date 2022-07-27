#ifndef XMLMODELS_H
#define XMLMODELS_H

#include "xmlmodel.h"
#include "xmlsgroupmodel.h"

class XmlBaseModel : public XmlModel
{
public:
    explicit XmlBaseModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// SIGNALS

class XmlSignalsModel : public XmlModel
{
public:
    explicit XmlSignalsModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// SECTIONS

class XmlSectionTabsModel : public XmlModel
{
public:
    explicit XmlSectionTabsModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlSectionsModel : public XmlModel
{
public:
    explicit XmlSectionsModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual const QModelIndex append(const QStringList &input) override;
};

class XmlSectionModel : public XmlModel
{
public:
    explicit XmlSectionModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual const QModelIndex append(const QStringList &input) override;
};

/// ALARMS

class XmlAlarmsModel : public XmlModel
{
public:
    explicit XmlAlarmsModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// JOURNALS

class XmlWorkJoursModel : public XmlModel
{
public:
    explicit XmlWorkJoursModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlMeasJoursModel : public XmlModel
{
public:
    explicit XmlMeasJoursModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// DATA EXCHANGE PROTOCOLS

class XmlModbusModel : public XmlModel
{
public:
    explicit XmlModbusModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlProtocomModel : public XmlModel
{
public:
    explicit XmlProtocomModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlIecModel : public XmlModel
{
public:
    explicit XmlIecModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// S2 CONFIG

class XmlConfigModel : public XmlModel
{
public:
    explicit XmlConfigModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

#endif // XMLMODELS_H
