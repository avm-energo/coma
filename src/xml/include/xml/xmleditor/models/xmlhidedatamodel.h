#pragma once

#include <xml/xmleditor/models/xmlmodel.h>

struct SGroupHideData;
struct S2RecordHideData;

/// \brief Class for storing hiding data.
class XmlHideDataModel final : public XmlModel
{
private:
    /// \brief Parsing node with specified name to the integer variable.
    /// \ingroup Internal parsing
    void parseInteger(const QDomNode &source, const QString &nodeName, int &dest);
    /// \brief Parsing node with specified name to the string variable.
    /// \ingroup Internal parsing
    void parseText(const QDomNode &source, const QString &nodeName, QString &dest);
    /// \brief Parsing 'string-array' node to the string list.
    /// \ingroup Internal parsing
    void parseStringArray(const QDomNode &source, QStringList &dest);

    /// \brief Parsing XML DOM node to SGroupHideData structure.
    /// \ingroup SGroup
    SGroupHideData parseSGroupData(QDomNode &node);
    /// \brief Converting input string list to the SGroupHideData structure.
    /// \ingroup SGroup
    SGroupHideData convertToSGroupData(const QStringList &input);
    /// \brief Converting input SGroupHideData structure to the string list.
    /// \ingroup SGroup
    QStringList convertFromSGroupData(const SGroupHideData &input);
    /// \brief Creates <sgroup> node by the model data.
    /// \ingroup SGroup
    QDomElement makeSGroupNode(QDomDocument &doc);

    /// \brief Parsing XML DOM node to S2RecordHideData structure.
    /// \ingroup S2Records
    S2RecordHideData parseS2RecordData(QDomNode &node);
    /// \brief Creates <record> node for 's2files.xml' by the model data.
    /// \ingroup S2Records
    QDomElement makeS2RecordsNode(QDomDocument &doc);
    /// \brief Converting input S2RecordHideData structure to the string list.
    /// \ingroup S2Records
    QStringList convertFromS2RecordData(const S2RecordHideData &input);
    /// \brief Converting input string list to the S2RecordHideData structure.
    /// \ingroup S2Records
    S2RecordHideData convertToS2RecordData(const QStringList &input);

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
