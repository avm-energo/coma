#include "valuemodel.h"

ValueModel::ValueModel(QObject *parent) : ETableModel(parent) { }

QVariant ValueModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        int row = index.row();
        int column = index.column();
        if ((row < maindata.size()) && (column < hdr.size()))
        {
            switch (role)
            {
            case Qt::DisplayRole:
            case Qt::EditRole:
                switch (maindata.at(row)->uData(column).toInt()) {
                case DataBlock::OUTVALUEINT:
                    quint32 *value = maindata.at(row)->data(column);
                    break;
                case DataBlock::OUTVALUEHEX:
                    textToDisplay = QString::number(index.data().toUInt(), 16);
                    break;
                case DataBlock::OUTVALUEFLOAT0:
                case DataBlock::OUTVALUEFLOAT1:
                case DataBlock::OUTVALUEFLOAT2:
                case DataBlock::OUTVALUEFLOAT3:
                case DataBlock::OUTVALUEFLOAT4:
                case DataBlock::OUTVALUEFLOAT5:
                    textToDisplay = QString::number(index.data().toFloat(), 'g', dataFormat - 2);
                    break;
                case DataBlock::OUTVALUEDOUBLE:
                    textToDisplay = QString::number(index.data().toDouble(), 'e', 4);
                    break;
                case DataBlock::OUTVALUESTRING:
                default:
                    textToDisplay = index.data().toString();
                    break;
                }
                return maindata.at(row)->data(column);
            case Qt::FontRole:
                return QVariant::fromValue(QFont(maindata.at(row)->font(column)));
            case Qt::ForegroundRole:
                return QVariant::fromValue(QColor(maindata.at(row)->color(column)));
            case Qt::DecorationRole:
                return QVariant::fromValue(QIcon(maindata.at(row)->icon(column)));
            case Qt::TextAlignmentRole:
                return maindata.at(row)->TextAlignment(column);
            case DataFormatRole:
                return maindata.at(row)->uData(column);
            case AdditionalDataRole:
                return qobject_cast<QVariant>(maindata.at(row)->aData(column));
            }
        }
    }
    return QVariant();
}

bool ValueModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && value.isValid())
    {
        switch (role)
        {
        case Qt::EditRole:
            if (index.column() < hdr.size())
            {
                maindata.last()->setData(index.column(), value.toString());
                // maindata.at(index.row())->setData(index.column(), value.toString()); // пишем само значение
                return true;
            }
            break;
        case Qt::ForegroundRole:
            maindata.last()->setColor(index.column(), value.value<QColor>());
            return true;
        case Qt::FontRole:
            maindata.last()->setFont(index.column(), value.value<QFont>());
            return true;
        case Qt::DecorationRole:
            maindata.last()->setIcon(index.column(), value.value<QIcon>());
            return true;
        case Qt::TextAlignmentRole:
            maindata.last()->setTextAlignment(index.column(), value.toInt());
            return true;
        case ValueModel::DataFormatRole:
            maindata.last()->setUData(index.column(), value);
            return true;
        case ValueModel::AdditionalDataRole:
            maindata.last()->setAData(index.column(), qvariant_cast<QObject *>(value));
        }
    }
    return false;
}

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
        QVariant *value = static_cast<QVariant *>(it.value());
        setData(*pindex, *value, Qt::EditRole);
    }
}

void ValueModel::updateFromModel()
{
    for (QMap<QPersistentModelIndex *, void *>::iterator it = m_valuePtrMap.begin(); it != m_valuePtrMap.end(); ++it)
    {
        QPersistentModelIndex *pindex = it.key();
        QVariant *value = static_cast<QVariant *>(it.value());
        *value = data(*pindex, Qt::DisplayRole);
    }
}
