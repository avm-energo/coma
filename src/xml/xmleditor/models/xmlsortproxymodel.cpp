#include "xmlsortproxymodel.h"

#include "xmlmodel.h"

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

const QModelIndex XmlSortProxyModel::append(const QStringList &input)
{
    auto srcIndex = qobject_cast<XmlModel *>(sourceModel())->append(input);
    auto curIndex = mapFromSource(srcIndex);
    return curIndex;
}

bool XmlSortProxyModel::remove(int row)
{
    auto srcIndex = mapToSource(index(row, 0));
    auto srcModel = qobject_cast<XmlModel *>(sourceModel());
    return srcModel->remove(srcIndex.row());
}
