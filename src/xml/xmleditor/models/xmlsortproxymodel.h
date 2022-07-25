#ifndef XMLSORTPROXYMODEL_H
#define XMLSORTPROXYMODEL_H

#include <QSortFilterProxyModel>

class XmlSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit XmlSortProxyModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::UserRole + 1) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::UserRole + 1) override;
};

#endif // XMLSORTPROXYMODEL_H
