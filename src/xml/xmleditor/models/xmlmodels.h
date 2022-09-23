#ifndef XMLMODELS_H
#define XMLMODELS_H

#include "xmlsgroupmodel.h"

/// \brief Class of XML model that stores other XML models as items.
class XmlContainerModel : public XmlModel
{
private:
    QString getModelTagName() const;

public:
    explicit XmlContainerModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual void create(const QStringList &saved, int *row) override;
    virtual QDomElement toNode(QDomDocument &doc) override;
};

/// \brief Class of XML model that stores data strings as items.
class XmlDataModel : public XmlModel
{
private:
    std::tuple<QString, QString, std::function<void(QDomDocument &, QDomElement &, int &)>> getModelSettings();

public:
    explicit XmlDataModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual QDomElement toNode(QDomDocument &doc) override;
};

#endif // XMLMODELS_H
