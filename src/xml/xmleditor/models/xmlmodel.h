#ifndef XMLMODEL_H
#define XMLMODEL_H

#include <QAbstractTableModel>
#include <QtXml>

constexpr int ModelNodeRole = 0x0105; ///< Role for setting node with submodule

namespace tags
{
constexpr char res[] = "resources";
constexpr char desc[] = "desc";
constexpr char sigs[] = "signals";
constexpr char sig[] = "signal";
constexpr char checks[] = "checks";
constexpr char groups[] = "groups";
constexpr char group[] = "group";
constexpr char check[] = "check";
constexpr char header[] = "header";
constexpr char record[] = "record";
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
}

namespace tags
{
/// \brief Enumeration for saving type of submodule
enum GroupTypes : quint16
{
    None = 0,
    Resources,
    Signals,
    Checks,
    Groups,
    Check,
    Alarms,
    CritAlarms,
    WarnAlarms,
    InfoAlarms,
    Journals,
    WorkJours,
    MeasJours,
    Modbus,
    Protocom,
    IEC60870,
    Config
};
}
using ModelType = tags::GroupTypes;

class XmlModel;

/// \brief Структура для хранения узла хранимой подмодели
struct ModelNode
{
    XmlModel *modelPtr;
    ModelType modelType;
};
Q_DECLARE_METATYPE(ModelNode);

/// \brief Базовый абстрактный класс для моделей XML узлов
class XmlModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    void checkChilds(QDomNode &child, int &row);

protected:
    int mRows, mCols;
    QHash<QModelIndex, QVariant> mHashTable;
    QHash<int, QVariant> mNodes;
    QHash<int, QVariant> horizontalHeaders;

    void parseTag(QDomNode &node, QString tagName, int row, int col);
    void parseAttribute(QDomNode &node, QString attrName, int row, int col);

public:
    static const std::map<QString, ModelType> types;       ///< Types Map with enumeration, key = name of group type
    static const std::map<ModelType, QStringList> headers; ///< Settings Map, key = group type enumeration

    explicit XmlModel(int rows, int cols, QObject *parent = nullptr);
    virtual QVariant data(const QModelIndex &index, int nRole = Qt::UserRole + 1) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &val, int nRole = Qt::UserRole + 1) override;
    virtual int rowCount(const QModelIndex &index = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &index = QModelIndex()) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, //
        const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void setHorizontalHeaderLabels(const QStringList &labels);

    void setDataNode(bool isChildModel, QDomNode &root);
    virtual void parseNode(QDomNode &node, int &row) = 0;
};

#endif // XMLMODEL_H
