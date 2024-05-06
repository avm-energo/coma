#pragma once

#include <xml/xmleditor/models/xmlmodel.h>

/// \brief Class of XML model that stores other XML models as items.
class XmlContainerModel final : public XmlModel
{
    Q_OBJECT
private:
    /// \brief Returns name of XML DOM node in dependency of model type.
    QString getModelTagName() const;

public:
    explicit XmlContainerModel(int rows, int cols, ModelType type, QObject *parent = nullptr);

    /// \brief Parsing input XML nodes of file in model items.
    void parseNode(QDomNode &node, int &row) override;
    /// \brief Slot for inserting a new item in the model.
    void create(const QStringList &saved, int *row) override;
    /// \brief Creates XML DOM node representation of current model.
    QDomElement toNode(QDomDocument &doc) override;
    /// \brief Returns data from the model for given row.
    /// \details Override needed for XmlResourceModel.
    QStringList getRowData(const int row) override;

public slots:
    /// \brief Slot for updating an item's data in the model.
    /// \details Override needed for XmlResourceModel.
    void update(const QStringList &saved, const int row) override;
};
