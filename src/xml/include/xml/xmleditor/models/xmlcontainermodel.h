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
    virtual void parseNode(QDomNode &node, int &row) override;
    /// \brief Slot for inserting a new item in the model.
    virtual void create(const QStringList &saved, int *row) override;
    /// \brief Creates XML DOM node representation of current model.
    virtual QDomElement toNode(QDomDocument &doc) override;

public slots:
    /// \brief Slot for receiving a request from dialog and emits signal with response.
    /// \details Override needed for XmlResourceModel.
    virtual void getDialogRequest(const int row) override;
    /// \brief Slot for updating an item's data in the model.
    /// \details Override needed for XmlResourceModel.
    virtual void update(const QStringList &saved, const int row) override;
};
