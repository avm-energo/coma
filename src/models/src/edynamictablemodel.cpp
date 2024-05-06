#include "models/edynamictablemodel.h"

#include <QDebug>

EDynamicTableModel::EDynamicTableModel(QObject *parent) : QAbstractTableModel(parent), dataCount(0)
{
}

QVariant EDynamicTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole || role == Qt::EditRole) && (section < hdr.size()))
        return hdr.at(section);
    return QVariant();
}

bool EDynamicTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation != Qt::Horizontal)
        // Велосипед т.к. предусмотрено только одно использование таблицы
        return false;
    if (role != Qt::DisplayRole)
        return false;
    QStringList *header = &hdr;
    if (section >= columnCount())
        header->append(value.toString());
    else
        header->replace(section, value.toString());

    return true;
}

QVariant EDynamicTableModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        int row = index.row();
        int column = index.column();
        if ((row < int(maindata.size())) && (column < hdr.size()))
        {
            switch (role)
            {
            case Qt::DisplayRole:
            case Qt::EditRole:
            {
                if (column < ColFormat.size())
                {
                    if (ColFormat.at(column) < 10)
                    {
                        QVariant cellvalue = maindata.at(row)->data(column, role);
                        bool ok;
                        double celldblvalue = cellvalue.toDouble(&ok);
                        if (ok)
                        {
                            cellvalue = QString::number(celldblvalue, 'f', ColFormat.at(column));
                            return cellvalue;
                        }
                    }
                }
            }
            default:
                return maindata.at(row)->data(column, role);
            }
        }
    }

    return QVariant();
}

// value должен представлять из себя запись вида: <value>.<links>, где links - вспомогательное поле, определяющее
// порядок работы с полем - подставляемый делегат, ссылку на списки и формат отображения

bool EDynamicTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && value.isValid())
    {
        maindata.back()->setData(index.column(), value, role);
    }
    return false;
}

Qt::ItemFlags EDynamicTableModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
    return Qt::NoItemFlags;
}

QModelIndex EDynamicTableModel::index(int row, int column, const QModelIndex &index) const
{
    Q_UNUSED(index)
    return createIndex(row, column);
}

bool EDynamicTableModel::insertColumns(int position, int columns, const QModelIndex &index)
{
    Q_UNUSED(index)
    if (columns > 0)
    {
        beginInsertColumns(QModelIndex(), position, position + columns - 1);
        // если в модели есть какие-то данные, то уже нельзя менять размерность таблицы по столбцам
        if (!isEmpty())
            return false;
        hdr.reserve(position + 1);
        ColFormat.reserve(position + 1);

        for (int i = 0; i < columns; i++)
        {
            hdr.insert(position, "");
            ColFormat.insert(position, noColFormat);
        }
        endInsertColumns();
    }
    return true;
}

bool EDynamicTableModel::removeColumns(int position, int columns, const QModelIndex &index)
{
    if (columns > 0)
    {
        beginRemoveColumns(index, position, position + columns - 1);
        if (!isEmpty())
            return false;
        if ((position + columns) > hdr.size())
            return false;
        for (int i = 0; i < columns; i++)
            hdr.removeAt(position);
        endRemoveColumns();
    }
    return true;
}

bool EDynamicTableModel::insertRows(int row, int count, const QModelIndex &index)
{
    Q_UNUSED(index)
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; i++)
    {
        ETableRow *item = new ETableRow(this);

        if (i >= rowCount() && i <= rowCount())
        {
            maindata.push_back(item);
        }
        else
            maindata.insert(maindata.begin() + row, item);
    }
    endInsertRows();
    return true;
}

bool EDynamicTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    beginRemoveRows(index, position, position + rows - 1);
    if ((position + rows) > int(maindata.size()))
        return false;
    for (int i = 0; i < rows; i++)
    {
        ETableRow *item = maindata.at(position);
        maindata.erase(maindata.begin() + position);
        // maindata.removeAt(position);
        delete item;
    }
    endRemoveRows();
    return true;
}

int EDynamicTableModel::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return hdr.size();
}

int EDynamicTableModel::rowCount(const QModelIndex &index) const
{
    //   Q_UNUSED(index)
    return index.isValid() ? 0 : int(dataCount);
    // return int(maindata.size());
}

/// Fill all model directly
void EDynamicTableModel::fillModel(const QVector<QVector<QVariant>> &lsl)
{
    beginResetModel();
    //  beginInsertRows(index(0, 0, QModelIndex()), 0, lsl.size());
    for (int i = 0; i < lsl.size(); ++i)
    {
        const auto rowVector = lsl.at(i);
        int lastEntry = int(maindata.size());
        int count = 1;
        int row = lastEntry;
        ETableRow *item = new ETableRow(this);
        for (int j = 0; j < rowVector.size(); ++j)
            item->setData(j, rowVector.at(j), Qt::EditRole);
        if (count >= rowCount() && count <= rowCount())
        {
            maindata.push_back(item);
        }
        else
            maindata.insert(maindata.begin() + row, item);
    }
    endResetModel();
    // endInsertRows();
}

bool EDynamicTableModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid())
        return false;
    return (dataCount < maindata.size());
}

void EDynamicTableModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
        return;
    int remainder = int(maindata.size() - int(dataCount));
    int itemsToFetch = qMin(fetchStep, remainder);

    if (itemsToFetch <= 0)
        return;

    beginInsertRows(QModelIndex(), int(dataCount), int(dataCount + itemsToFetch - 1));

    dataCount += itemsToFetch;

    endInsertRows();

    // qDebug() << "Fetched: " << itemsToFetch;
}

bool EDynamicTableModel::isEmpty() const
{
    return maindata.empty();
}

void EDynamicTableModel::setColumnFormat(int column, int format)
{
    while (column >= ColFormat.size())
        ColFormat.append(noColFormat);
    ColFormat.replace(column, format);
}

void EDynamicTableModel::setHorizontalHeaderLabels(const QStringList hdrl)
{
    for (int var = 0; var < hdrl.size(); ++var)
    {
        setHeaderData(var, Qt::Horizontal, hdrl.at(var), Qt::DisplayRole);
    }
}

void EDynamicTableModel::setHeaders(const QStringList hdrl)
{
    for (int var = 0; var < hdrl.size(); ++var)
    {
        setHeaderData(var, Qt::Horizontal, hdrl.at(var), Qt::DisplayRole);
    }
}

void EDynamicTableModel::clearModel()
{
    beginResetModel();
    if (!maindata.empty())
        qDeleteAll(maindata);
    hdr.clear();
    maindata.clear();
    ColFormat.clear();
    dataCount = 0;
    qDebug() << "Model was cleared";
    endResetModel();
}
