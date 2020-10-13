#ifndef VALUEMODEL_H
#define VALUEMODEL_H

#include "../gen/datablock.h"
#include <QAbstractTableModel>

#include <QObject>

class ValueModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum TableModelRoles
    {
        DataFormatRole = Qt::UserRole,
        AdditionalDataRole = Qt::UserRole + 1
    };

    ValueModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    int columnCount(const QModelIndex &index = QModelIndex()) const override;
    bool insertColumns(int position, int columns, const QModelIndex &index = QModelIndex()) override;
    bool removeColumns(int position, int columns, const QModelIndex &index = QModelIndex()) override;
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

    void setValueData(const QModelIndex &index, void *valuePtr);
    void setModel(const QList<ValueItem *> &vl, int dataColumns);
    void updateModel();
    void updateFromModel();

private:
    QMap<QPersistentModelIndex *, void *> m_valuePtrMap;
    QVector<QVector<ValueItem *>> m_data;
};

#endif // VALUEMODEL_H
