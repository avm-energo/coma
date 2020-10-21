#pragma once

#include <../gen/error.h>
#include <QAbstractItemModel>
typedef QPair<QStringList, QtMsgType> ErrorPair;
typedef QList<ErrorPair> ErrorList;

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

    bool insertRow(int row, ErrorPair *item);
    inline bool insertRow(int row, const QModelIndex &parent = QModelIndex())
    {
        return insertRows(row, 1, parent);
    }
    bool appendRows(const ErrorList &list);
    inline bool appendRow(ErrorPair *pair)
    {
        return insertRow(rowCount(), pair);
    }

    void appendRow(Error::ErMsg ermsg);
    void initModel();

protected:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QList<ErrorPair *> m_dataptr;
    QStringList m_hdr;
};
