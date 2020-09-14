#include "valuemodel.h"

ValueModel::ValueModel(QObject *parent) : ETableModel(parent) { }

void ValueModel::setValueData(const QModelIndex &index, void *valuePtr)
{
    QPersistentModelIndex *pindex = new QPersistentModelIndex(index);
    m_valuePtrMap[pindex] = valuePtr;
}

void ValueModel::updateModel()
{
    for (QMap<QPersistentModelIndex *, void *>::iterator it = m_valuePtrMap.begin(); it != m_valuePtrMap.end(); ++it)
    {
        QPersistentModelIndex *pindex = it.key();
        float *value = static_cast<float *>(it.value());
        setData(*pindex, *value, Qt::EditRole);
    }
}

void ValueModel::updateFromModel()
{
    for (QMap<QPersistentModelIndex *, void *>::iterator it = m_valuePtrMap.begin(); it != m_valuePtrMap.end(); ++it)
    {
        QPersistentModelIndex *pindex = it.key();
        float *value = static_cast<float *>(it.value());
        *value = data(*pindex, Qt::DisplayRole).toFloat();
    }
}
