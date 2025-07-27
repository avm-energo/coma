#pragma once

#include <xml/xmleditor/models/baseeditormodel.h>

constexpr int ModelNodeRole = 0x0105; ///< Role for setting node with submodule

// Опережающее объявление
class XmlModel;

/// \brief Structure for storaging child XML model node.
struct ChildModelNode
{
    XmlModel *m_model = nullptr;
    ModelType m_type = ModelType::None;
};
Q_DECLARE_METATYPE(ChildModelNode);

/// \brief Base abstract XML model class.
class XmlModel : public BaseEditorModel
{
    Q_OBJECT
protected:
    /// \brief Parses given child node in current XML model.
    /// \details For given child node applying virtual function parseNode. If name
    /// of given node is in static types map, then for this item will be creating new
    /// XML model with help static members of ModelFabric class.
    /// \see parseNode, setDataNode
    void parseDataNode(QDomNode &child, int &row);

    /// \brief Parses given tag from given XML DOM node in XML model at given index.
    /// \details Frequently called by implementations of parseNode virtual function.
    /// \see parseAttribute, parseNode
    void parseTag(QDomNode &node, const QString &tagName, int row, int col, //
        const QString &defValue = "", bool isInt = false);

    /// \brief Parses given attribute from given XML DOM node in XML model at given index.
    /// \details Frequently called by implementations of parseNode virtual function.
    /// \see parseTag, parseNode
    void parseAttribute(QDomNode &node, const QString &attrName, int row, int col, const QString &defValue = "");

public:
    static const std::map<QString, ModelType> s_types;       ///< Types Map with enumeration, key = name of group type
    static const std::map<ModelType, QStringList> s_headers; ///< Settings Map, key = group type enumeration

    /// \brief Base XML model class ctor.
    explicit XmlModel(int rows, int cols, ModelType type, QObject *parent = nullptr);

    /// \brief Returns the data stored under the given role for the item referred to by the index.
    /// \details This function reimplement and override the same
    /// function from base class QAbstractItemModel.
    /// \see setData, setHeaderData
    virtual QVariant data(const QModelIndex &index, int nRole = Qt::DisplayRole) const override;
    /// \brief Sets the role data for the item at index to value.
    /// \details This function reimplement and override the same
    /// function from base class QAbstractItemModel.
    /// \returns Returns true if successful; otherwise returns false.
    /// \see data, headerData
    virtual bool setData(const QModelIndex &index, const QVariant &val, int nRole = Qt::EditRole) override;

    /// \brief Slot for inserting a new item in the model.
    virtual void create(const QStringList &saved, int *row) override;
    /// \brief Slot for updating an item's data in the model.
    virtual void update(const QStringList &saved, const int row) override;
    /// \brief Slot for deleting an exisiting item in the model.
    virtual void remove(const int row) override;

    /// \brief Parses given XML DOM node in current XML model.
    /// \details For each child node of given XML DOM node applying function parseDataNode.
    /// \see parseDataNode, parseNode
    void setDataNode(bool isChildModel, QDomNode &root);

    virtual QDomElement toNode(QDomDocument &doc) = 0;
    virtual void parseNode(QDomNode &node, int &row) = 0;
};
