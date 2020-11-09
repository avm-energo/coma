#include "valuemodel.h"

ValueModel::ValueModel(QObject *parent) : QAbstractTableModel(parent)
{
}

QVariant ValueModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
}

bool ValueModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(value)
    Q_UNUSED(role)
    return true;
}

QVariant ValueModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:
        {
            int row = index.row();
            int column = index.column();
            if (row < m_data.size())
            {
                QVector<ValueItem *> vv = m_data.at(row);
                if (column < vv.size())
                {
                    QVariant value;
                    value.setValue(vv.at(column));
                    return value;
                }
            }
        }
        break;
        }
    }
    return QVariant();
}

bool ValueModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && value.isValid())
    {
        int row = index.row();
        int column = index.column();
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            if (row >= m_data.size())
                m_data.resize(row + 1);
            QVector<ValueItem *> vv = m_data.at(row);
            if (column >= vv.size())
                vv.resize(column + 1);
            vv.replace(column, qvariant_cast<ValueItem *>(value));
            // m_data.at(index.row())->setData(index.column(), value.toString()); // пишем само значение
            m_data.replace(row, vv);
            return true;
        }
        break;
        }
    }
    return false;
}

Qt::ItemFlags ValueModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
    return Qt::NoItemFlags;
}

int ValueModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return m_data.size();
}

void ValueModel::setValueData(const QModelIndex &index, QVariant *valuePtr)
{
    QPersistentModelIndex *pindex = new QPersistentModelIndex(index);
    m_valuePtrMap[pindex] = valuePtr;
}

void ValueModel::setModel(const QList<ValueItem *> &vl, int dataColumns)
{
    int currow = 0;
    int curcol = 0;
    foreach (ValueItem *vi, vl)
    {
        QVariant value;
        value.setValue(vi);
        setData(index(currow, curcol++), vi->name(), Qt::EditRole);
        setData(index(currow, curcol), value, Qt::EditRole);
        setData(index(currow, curcol), vi->tooltip(), Qt::ToolTipRole);
        setValueData(index(currow, curcol++), vi->dataPtr());
        if (curcol > dataColumns)
        {
            currow++;
            curcol = 0;
        }
    }
    //    m_VModel->setData(m_VModel->index(m_curModelRow, m_curModelColumn++), dd.name);
    //    m_VModel->setData(
    //        m_VModel->index(m_curModelRow, m_curModelColumn), DataFormat::OUTVALUEINT, ETableModel::DataFormatRole);
    //    m_VModel->setData(m_VModel->index(m_curModelRow, m_curModelColumn), qobject_cast<QVariant>(dd.adddata),
    //        ETableModel::AdditionalDataRole);
    //    m_VModel->setData(m_VModel->index(m_curModelRow, m_curModelColumn), dd.tooltip, Qt::ToolTipRole);
    //    m_VModel->setValueData(m_VModel->index(m_curModelRow, m_curModelColumn++), dd.dataptr);
    //    if (m_curModelColumn > MAX_VALUEMODEL_COLUMNS)
    //    {
    //        m_curModelRow++;
    //        m_curModelColumn = 0;
    //    }
}

void ValueModel::updateModel()
{
    for (QMap<QPersistentModelIndex *, QVariant *>::iterator it = m_valuePtrMap.begin(); it != m_valuePtrMap.end();
         ++it)
    {
        QPersistentModelIndex *pindex = it.key();
        QVariant *value = static_cast<QVariant *>(it.value());
        setData(*pindex, *value, Qt::EditRole);
    }
}

void ValueModel::updateFromModel()
{
    for (QMap<QPersistentModelIndex *, QVariant *>::iterator it = m_valuePtrMap.begin(); it != m_valuePtrMap.end();
         ++it)
    {
        QPersistentModelIndex *pindex = it.key();
        QVariant *value = static_cast<QVariant *>(it.value());
        *value = data(*pindex, Qt::DisplayRole);
    }
}
