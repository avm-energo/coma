#pragma once
#include "errorprotocolitem.h"

#include <QAbstractItemModel>

struct ErrorMsg;
class ErrorProtocolModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ErrorProtocolModel(QObject *parent = 0);
    ~ErrorProtocolModel();
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;

    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    int columnCount(const QModelIndex &index = QModelIndex()) const override;

    bool insertRow(int row, ErrorProtocolItem *item);
    inline bool insertRow(int row, const QModelIndex &parent = QModelIndex())
    {
        return insertRows(row, 1, parent);
    }
    inline bool appendRow(ErrorProtocolItem *pair)
    {
        return insertRow(rowCount(), pair);
    }

    void appendRows(const std::vector<ErrorMsg> &msgs, int newItems);
    void initModel();

protected:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QList<ErrorProtocolItem *> m_dataptr;
    QStringList m_hdr;
};
