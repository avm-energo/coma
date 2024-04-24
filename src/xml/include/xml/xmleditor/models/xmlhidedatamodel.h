#pragma once

#include <xml/xmleditor/models/xmlmodel.h>

constexpr int SGroupDataRole = 0x0106; ///< Role for hiding data SGroupHideData.

/// \brief Structure, that stores a hiding data.
struct SGroupHideData
{
    int count = 1;
    QString tooltip = "";
    QString view = "float";
    QStringList array = {};
};
Q_DECLARE_METATYPE(SGroupHideData);

/// \brief Class for storing hiding data.
class XmlHideDataModel final : public XmlModel
{
private:
    /// \brief Parsing XML DOM node to SGroupHideData structure.
    SGroupHideData parseSGroupData(QDomNode &node);
    /// \brief Converting input string list to SGroupHideData structure.
    SGroupHideData convertToSGroupData(const QStringList &input);
    /// \brief Filling <sgroup> node by the model data.
    void fillSGroupNode(QDomDocument &doc, QDomElement &sgroupNode);

public:
    explicit XmlHideDataModel(int rows, int cols, ModelType type, QObject *parent = nullptr);

    /// \brief Parsing input XML nodes of file in model items.
    void parseNode(QDomNode &node, int &row) override;
    /// \brief Creates XML DOM node representation of this model.
    QDomElement toNode(QDomDocument &doc) override;
    /// \brief Returns data from the model for given row.
    QStringList getRowData(const int row) override;

public slots:
    /// \brief Slot for creating data in model.
    void create(const QStringList &saved, int *row) override;
    /// \brief Slot for updating an item's data in the model (including hiding data).
    void update(const QStringList &saved, const int row) override;
};
