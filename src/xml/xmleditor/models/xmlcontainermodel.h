#pragma once

#include "xmlmodel.h"

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
