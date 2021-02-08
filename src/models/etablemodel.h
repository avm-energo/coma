#ifndef ETABLEMODEL_H
#define ETABLEMODEL_H

#include "etablerow.h"

#include <QAbstractTableModel>
namespace
{
// 11 is the number more than 10 i.e. no format for column
constexpr int noColFormat = 11;
}

class ETableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ETableModel(QObject *parent = 0);
    ~ETableModel();

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
    // получение индекса элемента в заголовке, который содержит текст hdrtext
    int headerPosition(
        QVariant hdrtext, Qt::Orientation orientation = Qt::Horizontal, int role = Qt::DisplayRole) const;
    // добавление новой колонки с текстом в заголовке hdrtext для варианта двух столбцов
    void addColumn(const QString hdrtext);
    // добавление строки
    void addRow();
    void setRowAttr(int fcset = 0, int icon = -1);
    void clearModel();
    void fillModel(QVector<QVector<QVariant>> &);
    // выдать значения по столбцу column в выходной QStringList
    QStringList takeColumn(int column) const;
    // выдать значения по строке row в выходной QStringList
    QStringList takeRow(int row) const;
    void setRowTextAlignment(int row, int alignment);
    bool isEmpty() const;
    // format is precision of the double, set num above 10 to set no format
    void setColumnFormat(int column, int format);
    void setHorizontalHeaderLabels(const QStringList hdrl);
    void setHeaders(const QStringList hdrl);
    void addRowWithData(const QVector<QVariant> &vl);

signals:
    void clear();
    void pushProgress(int);
    void pushMaxProgress(int);

protected slots:

private:
    QList<ETableRow *> maindata;
    QStringList hdr;
    QList<int> ColFormat;
};

#endif // ETABLEMODEL_H
