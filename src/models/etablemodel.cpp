#include "etablemodel.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QMutex>
//#include <QProgressDialog>
// ######################################## Переопределение методов QAbstractTableModel
// ####################################
// 11 is the number more than 10 i.e. no format for column
static constexpr int noColFormat = 11;

ETableModel::ETableModel(QObject *parent) : QAbstractTableModel(parent)
{
}

ETableModel::~ETableModel()
{
}

QVariant ETableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole || role == Qt::EditRole) && (section < hdr.size()))
        return hdr.at(section);
    return QVariant();
}

bool ETableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    //    if ((section < 0) || ((orientation == Qt::Horizontal) && (section >= columnCount()))
    //        || ((orientation == Qt::Vertical) && (section >= rowCount())))
    //    {
    //        return false;
    //    }
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

QVariant ETableModel::data(const QModelIndex &index, int role) const
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

/*ETableItem ETableModel::item(const QModelIndex &index)
{
    return maindata.at(index.row());
} */

// value должен представлять из себя запись вида: <value>.<links>, где links - вспомогательное поле, определяющее
// порядок работы с полем - подставляемый делегат, ссылку на списки и формат отображения

bool ETableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && value.isValid())
    {
        maindata.back()->setData(index.column(), value, role);
    }
    return false;
}

Qt::ItemFlags ETableModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
    return Qt::NoItemFlags;
}

QModelIndex ETableModel::index(int row, int column, const QModelIndex &index) const
{
    Q_UNUSED(index)
    return createIndex(row, column);
}

bool ETableModel::insertColumns(int position, int columns, const QModelIndex &index)
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
        //        while (position > hdr.size())
        //        {
        //            hdr.append("");
        //            ColFormat.append(NOCOLFORMAT);
        //        }
        for (int i = 0; i < columns; i++)
        {
            hdr.insert(position, "");
            ColFormat.insert(position, noColFormat);
        }
        endInsertColumns();
    }
    return true;
}

bool ETableModel::removeColumns(int position, int columns, const QModelIndex &index)
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

bool ETableModel::insertRows(int row, int count, const QModelIndex &index)
{
    Q_UNUSED(index)
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; i++)
    {
        ETableRow *item = new ETableRow(this);
        //        for (int j = 0; j < hdr.size(); j++)
        //            item->setData(j, "");
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

bool ETableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    beginRemoveRows(index, position, position + rows - 1);
    if ((position + rows) > maindata.size())
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

int ETableModel::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return hdr.size();
}

int ETableModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)

    return int(maindata.size());
}

// ###################################### Свои методы ############################################

int ETableModel::headerPosition(QString hdrtext, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole || role == Qt::EditRole))
    {
        auto it = std::find_if(hdr.cbegin(), hdr.cend(), [&](const QString &s) { return s.contains(hdrtext); });
        if (it == hdr.cend())
            return -1;
        return hdr.indexOf(*it);
    }
    return -1;
}

void ETableModel::addColumn(const QString hdrtext)
{
    int lastEntry = columnCount();
    insertColumns(lastEntry, 1, QModelIndex());
    // hdr.append(hdrtext);
    //  hdr.reserve(lastEntry);
    hdr.insert(lastEntry, hdrtext);
    ColFormat.append(noColFormat);
}

void ETableModel::addRow()
{
    int lastEntry = maindata.size();
    insertRows(lastEntry, 1, QModelIndex());
}

/// Fill all model directly
void ETableModel::fillModel(QVector<QVector<QVariant>> &lsl)
{
    beginInsertRows(index(0, 0, QModelIndex()), 0, lsl.size());
    for (int i = 0; i < lsl.size(); ++i)
    {
        const auto rowVector = lsl.at(i);
        int lastEntry = maindata.size();
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
    endInsertRows();
}

// выдать значения по столбцу column в выходной QStringList

QStringList ETableModel::takeColumn(int column) const
{
    if (column > columnCount())
        return QStringList();
    QStringList tmpsl;
    for (int row = 0; row < rowCount(); row++)
        tmpsl.append(data(index(row, column, QModelIndex()), Qt::DisplayRole).toString());
    return tmpsl;
}

// выдать значения по строке row в выходной QStringList

QStringList ETableModel::takeRow(int row) const
{
    if (row > rowCount())
        return QStringList();
    QStringList tmpsl;
    for (int column = 0; column < columnCount(); column++)
        tmpsl.append(data(index(row, column, QModelIndex()), Qt::DisplayRole).toString());
    return tmpsl;
}

void ETableModel::setRowTextAlignment(int row, int alignment)
{
    while (row >= rowCount())
        insertRows(rowCount(), (row - rowCount()));
    for (int i = 0; i < columnCount(); ++i)
        setData(index(row, i, QModelIndex()), QVariant(alignment), Qt::TextAlignmentRole);
}

bool ETableModel::isEmpty() const
{
    return maindata.empty();
}

void ETableModel::setColumnFormat(int column, int format)
{
    while (column >= ColFormat.size())
        ColFormat.append(noColFormat);
    ColFormat.replace(column, format);
}

void ETableModel::setHorizontalHeaderLabels(const QStringList hdrl)
{
    for (int var = 0; var < hdrl.size(); ++var)
    {
        setHeaderData(var, Qt::Horizontal, hdrl.at(var), Qt::DisplayRole);
    }
}

void ETableModel::setHeaders(const QStringList hdrl)
{
    for (int var = 0; var < hdrl.size(); ++var)
    {
        setHeaderData(var, Qt::Horizontal, hdrl.at(var), Qt::DisplayRole);
    }
}

void ETableModel::addRowWithData(const QVector<QVariant> &vl)
{
    int currow = rowCount();
    addRow();
    hdr.reserve(vl.size());
    if (vl.size() > hdr.size()) // в переданном списке больше колонок, чем в модели
    {
        for (int i = hdr.size(); i < vl.size(); ++i)
            addColumn("");
    }
    for (int i = 0; i < vl.size(); ++i) // цикл по строкам
        setData(index(currow, i, QModelIndex()), vl.at(i), Qt::EditRole);
}

void ETableModel::clearModel()
{

    beginResetModel();
    if (!maindata.empty())
        qDeleteAll(maindata);
    hdr.clear();
    maindata.clear();
    ColFormat.clear();
    qDebug() << "Model was cleared";
    endResetModel();
}
