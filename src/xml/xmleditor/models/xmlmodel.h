#ifndef XMLMODEL_H
#define XMLMODEL_H

#include "ieditormodel.h"

constexpr int ModelNodeRole = 0x0105; ///< Role for setting node with submodule

/// \brief Namespace for storaging names of base XML tags.
namespace tags
{
// Main tags
constexpr char res[] = "resources";
constexpr char desc[] = "desc";
constexpr char sigs[] = "signals";
constexpr char tabs[] = "section-tabs";
constexpr char sections[] = "sections";
constexpr char section[] = "section";
constexpr char sgroup[] = "sgroup";
constexpr char alarms[] = "alarms";
constexpr char critical[] = "critical";
constexpr char info[] = "info";
constexpr char warning[] = "warning";
constexpr char journals[] = "journals";
constexpr char work[] = "work";
constexpr char meas[] = "meas";
constexpr char modbus[] = "modbus";
constexpr char protocom[] = "protocom";
constexpr char iec60870[] = "iec60870";
constexpr char config[] = "config";

// Specified tags
constexpr char sig[] = "signal";
constexpr char tab[] = "tab";

constexpr char id[] = "id";
constexpr char start_addr[] = "start-addr";
constexpr char count[] = "count";
constexpr char name_[] = "name";
// constexpr char res[] = "";
}

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
    void parseTag(QDomNode &node, const QString tagName, int row, int col);
    void parseAttribute(QDomNode &node, const QString attrName, int row, int col);

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

    virtual QDomElement *toNode(QDomDocument &doc);
    void setDataNode(bool isChildModel, QDomNode &root);
    virtual void parseNode(QDomNode &node, int &row) = 0;

public slots:
};

#endif // XMLMODEL_H
