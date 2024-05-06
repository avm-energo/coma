#pragma once

#include <xml/xmleditor/models/xmlmodel.h>

/// \brief Class of XML model that stores data strings as items.
class XmlDataModel final : public XmlModel
{
private:
    /// \brief Returns saving settings for XML model in dependency of model type.
    std::tuple<QString, QString, std::function<void(QDomDocument &, QDomElement &, int &)>> getModelSettings();
    /// \brief Parsing <highlights> node from alarm DOM nodes (<crit> and <warn> only).
    void parseAlarmHighlights(QDomNode &node, int row, int col);

public:
    explicit XmlDataModel(int rows, int cols, ModelType type, QObject *parent = nullptr);

    /// \brief Parsing input XML nodes of file in model items.
    virtual void parseNode(QDomNode &node, int &row) override;
    /// \brief Creates XML DOM node representation of current model.
    virtual QDomElement toNode(QDomDocument &doc) override;
};
