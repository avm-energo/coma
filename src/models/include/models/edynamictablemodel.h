#pragma once

#include <QAbstractTableModel>
#include <models/etablerow.h>

using Matrix = std::vector<ETableRow *>;
class EDynamicTableModel : public QAbstractTableModel
{
    // 11 is the number more than 10 i.e. no format for column
    static constexpr int noColFormat = 11;
    static constexpr int fetchStep = 1000;
    Q_OBJECT
public:
    explicit EDynamicTableModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &index = QModelIndex()) const override;
    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    int columnCount(const QModelIndex &index = QModelIndex()) const override;
    bool insertColumns(int position, int columns, const QModelIndex &index = QModelIndex()) override;
    bool removeColumns(int position, int columns, const QModelIndex &index = QModelIndex()) override;
    bool insertRows(int row, int count, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

    bool isEmpty() const;
    void setColumnFormat(int column, int format);
    void setHorizontalHeaderLabels(const QStringList hdrl);
    void setHeaders(const QStringList hdrl);
    void clearModel();
    void fillModel(const QVector<QVector<QVariant>> &);
    int itemCount() const
    {
        return int(maindata.size());
    }

protected:
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

private:
    Matrix maindata;
    Matrix::size_type dataCount;
    QStringList hdr;
    QList<int> ColFormat;
};
