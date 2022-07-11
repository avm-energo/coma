#ifndef XMLMODEL_H
#define XMLMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QtXml>

constexpr int ModelNodeRole = 0x0105;

enum GroupTypes : quint16
{
    None = 0,
    Resources,
    Check,
    Groups,
    Signals,
    Records
};

class XmlModel;
/// \brief Структура для хранения узла хранимой подмодели
struct ModelNode
{
    XmlModel *modelPtr;
    GroupTypes modelType;
};
Q_DECLARE_METATYPE(ModelNode);

/// \brief Базовый абстрактный класс для моделей XML узлов
class XmlModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class ModelFabric;

private:
    void checkChilds(QDomNode &child, int &row);

protected:
    int mRows, mCols;
    QHash<QModelIndex, QVariant> mHashTable;
    QHash<int, QVariant> mNodes;
    QHash<int, QVariant> horizontalHeaders;

public:
    static const std::map<QString, GroupTypes> types;        ///< Types Map with enumeration, key = name of group type
    static const std::map<GroupTypes, QStringList> settings; ///< Settings Map, key = group type enumeration

    XmlModel(int rows, int cols, QObject *parent = nullptr);
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
