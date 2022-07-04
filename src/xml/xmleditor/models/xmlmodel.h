#ifndef XMLMODEL_H
#define XMLMODEL_H

#include <QAbstractTableModel>

class XmlModel : QAbstractTableModel
{
    Q_OBJECT
private:
    int rows, cols;

public:
    XmlModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int nRole = Qt::UserRole + 1) const;
    bool setData(const QModelIndex &index, const QVariant &val, int nRole = Qt::UserRole + 1);
    int rowCount(const QModelIndex &index) const;
    int columnCount(const QModelIndex &index) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // XMLMODEL_H
