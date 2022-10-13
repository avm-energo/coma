#ifndef XMLMODEL_H
#define XMLMODEL_H

#include "../../xmltags.h"
#include "ieditormodel.h"

constexpr int ModelNodeRole = 0x0105; ///< Role for setting node with submodule

// Опережающее объявление
class XmlModel;

/// \brief Structure for storaging child XML model node.
struct ChildModelNode
{
    XmlModel *modelPtr = nullptr;
    ModelType modelType = ModelType::None;
};
Q_DECLARE_METATYPE(ChildModelNode);

/// \brief Base abstract XML model class.
class XmlModel : public IEditorModel
{
    Q_OBJECT
protected:
    void parseDataNode(QDomNode &child, int &row);
    void parseTag(QDomNode &node, const QString &tagName, int row, int col, const QString &defValue = "");
    void parseAttribute(QDomNode &node, const QString &attrName, int row, int col);

public:
    static const std::map<QString, ModelType> types;       ///< Types Map with enumeration, key = name of group type
    static const std::map<ModelType, QStringList> headers; ///< Settings Map, key = group type enumeration

    explicit XmlModel() = delete;
    explicit XmlModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual QVariant data(const QModelIndex &index, int nRole = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &val, int nRole = Qt::EditRole) override;
    virtual void create(const QStringList &saved, int *row) override;
    virtual void update(const QStringList &saved, const int &row) override;
    virtual void remove(const int &row) override;

    virtual QDomElement toNode(QDomDocument &doc) = 0;
    void setDataNode(bool isChildModel, QDomNode &root);
    virtual void parseNode(QDomNode &node, int &row) = 0;
};

#endif // XMLMODEL_H
