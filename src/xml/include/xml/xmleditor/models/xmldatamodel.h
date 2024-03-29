#pragma once

#include <xml/xmleditor/models/xmlmodel.h>

/// \brief Class of XML model that stores data strings as items.
class XmlDataModel : public XmlModel
{
private:
    std::tuple<QString, QString, std::function<void(QDomDocument &, QDomElement &, int &)>> getModelSettings();
    void parseAlarmHighlights(QDomNode &node, int row, int col);

public:
    explicit XmlDataModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual QDomElement toNode(QDomDocument &doc) override;
};
