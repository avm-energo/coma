#include "xmlsortproxymodel.h"

XmlSortProxyModel::XmlSortProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

QVariant XmlSortProxyModel::data(const QModelIndex &index, int role) const
{
    auto srcIndex = mapToSource(index);
    return sourceModel()->data(srcIndex, role);
}

bool XmlSortProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto srcIndex = mapToSource(index);
    return sourceModel()->setData(srcIndex, value, role);
}
