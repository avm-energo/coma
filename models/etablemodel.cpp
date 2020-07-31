#include "etablemodel.h"
#include <QDebug>
// ######################################## Переопределение методов QAbstractTableModel
// ####################################
#if _MSC_VER && !__INTEL_COMPILER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

ETableModel::ETableModel(QObject *parent) : QAbstractTableModel(parent)
{
    colors[0] = Qt::black;
    colors[1] = Qt::red;
    colors[2] = Qt::blue;
    colors[3] = Qt::darkRed;
    colors[4] = Qt::darkGreen;
    colors[5] = Qt::gray;
    QFont fontB = QFont("MS Sans Serif", -1, QFont::Bold);
    QFont fontN = QFont("MS Sans Serif", -1, QFont::Normal);
    fonts[0] = fonts[3] = fonts[5] = fontN;
    fonts[1] = fonts[2] = fonts[4] = fontB;
    icons[0] = QIcon("images/hr.png");
    icons[1] = QIcon("images/ok.png");
    icons[2] = QIcon("images/cross.png");
    // Изучить
    // hdr.clear();
    // maindata.clear();
}

ETableModel::~ETableModel() { }

QVariant ETableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole || role == Qt::EditRole) && (section < hdr.size()))
        return hdr.at(section);
    return QVariant();
}

bool ETableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}

QStringList ETableModel::Headers()
{
    if (!hdr.isEmpty())
        return hdr;
    return QStringList();
}

QVariant ETableModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        int row = index.row();
        int column = index.column();
        if ((row < maindata.size()) && (column < hdr.size()))
        {
            if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
            {
                if (column < ColFormat.size())
                {
                    if (ColFormat.at(column) < 10)
                    {
                        QString cellvalue = maindata.at(row)->data(column);
                        bool ok;
                        double celldblvalue = cellvalue.toDouble(&ok);
                        if (ok)
                        {
                            cellvalue = QString::number(celldblvalue, 'f', ColFormat.at(column));
                            return cellvalue;
                        }
                    }
                }
                return maindata.at(row)->data(column);
            }
            switch (role)
            {
            case Qt::FontRole:
                return QVariant::fromValue(QFont(maindata.at(row)->font(column)));
            case Qt::ForegroundRole:
                return QVariant::fromValue(QColor(maindata.at(row)->color(column)));
            case Qt::DecorationRole:
                return QVariant::fromValue(QIcon(maindata.at(row)->icon(column)));
            case Qt::TextAlignmentRole:
                return maindata.at(row)->TextAlignment(column);
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
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::EditRole:
            if (index.column() < hdr.size())
            {
                maindata.at(index.row())->setData(index.column(), value.toString()); // пишем само значение
                //                emit dataChanged(index, index);
                return true;
            }
            break;
        case Qt::ForegroundRole:
            maindata.at(index.row())->setColor(index.column(), value.value<QColor>());
            return true;
        case Qt::FontRole:
            maindata.at(index.row())->setFont(index.column(), value.value<QFont>());
            return true;
        case Qt::DecorationRole:
            maindata.at(index.row())->setIcon(index.column(), value.value<QIcon>());
            return true;
        case Qt::TextAlignmentRole:
            maindata.at(index.row())->setTextAlignment(index.column(), value.toInt());
            return true;
        }
    }
    return false;
}

Qt::ItemFlags ETableModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
    return 0;
}

QModelIndex ETableModel::index(int row, int column, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return createIndex(row, column);
}

bool ETableModel::insertColumns(int position, int columns, const QModelIndex &index)
{
    Q_UNUSED(index);
    if (columns > 0)
    {
        beginInsertColumns(QModelIndex(), position, position + columns - 1);
        if (!maindata
                 .isEmpty()) // если в модели есть какие-то данные, то уже нельзя менять размерность таблицы по столбцам
            return false;
        while (position > hdr.size())
        {
            hdr.append("");
            ColFormat.append(NOCOLFORMAT);
        }
        for (int i = 0; i < columns; i++)
        {
            hdr.insert(position, "");
            ColFormat.insert(position, NOCOLFORMAT);
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
        if (!maindata.isEmpty())
            return false;
        if ((position + columns) > hdr.size())
            return false;
        for (int i = 0; i < columns; i++)
            hdr.removeAt(position);
        endRemoveColumns();
    }
    return true;
}

bool ETableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);
    for (int i = 0; i < rows; i++)
    {
        ETableItem *item = new ETableItem();
        for (int j = 0; j < hdr.size(); j++)
            item->setData(j, "");
        if (i >= rowCount() && i <= rowCount())
        {
            maindata.append(item);
            qDebug("Row has been appended");
        }
        else
            maindata.insert(position, item);
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
        ETableItem *item = maindata.at(position);
        maindata.removeAt(position);
        delete item;
    }
    endRemoveRows();
    return true;
}

int ETableModel::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return hdr.size();
}

int ETableModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return maindata.size();
}

// ###################################### Свои методы ############################################

int ETableModel::getHeaderPosition(QVariant hdrtext, Qt::Orientation orientation, int role)
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole || role == Qt::EditRole))
        return hdr.indexOf(hdrtext.toString());
    return -1;
}

void ETableModel::addColumn(const QString hdrtext)
{
    int lastEntry = columnCount();
    insertColumns(lastEntry, 1, QModelIndex());
    hdr.replace(lastEntry, hdrtext);
    ColFormat.append(NOCOLFORMAT);
}

void ETableModel::addRow()
{
    int lastEntry = maindata.size();
    insertRows(lastEntry, 1, QModelIndex());
}

void ETableModel::fillModel(QVector<QVector<QVariant>> lsl)
{
    for (const auto i : lsl)
        AddRowWithData(i);
}

// выдать значения по столбцу column в выходной QStringList

QStringList ETableModel::cvalues(int column)
{
    if (column > columnCount())
        return QStringList();
    QStringList tmpsl;
    for (int row = 0; row < rowCount(); row++)
        tmpsl.append(data(index(row, column, QModelIndex()), Qt::DisplayRole).toString());
    return tmpsl;
}

// выдать значения по строке row в выходной QStringList

QStringList ETableModel::rvalues(int row)
{
    if (row > rowCount())
        return QStringList();
    QStringList tmpsl;
    for (int column = 0; column < columnCount(); column++)
        tmpsl.append(data(index(row, column, QModelIndex()), Qt::DisplayRole).toString());
    return tmpsl;
}

void ETableModel::SetRowTextAlignment(int row, int alignment)
{
    while (row >= rowCount())
        insertRows(rowCount(), (row - rowCount()));
    for (int i = 0; i < columnCount(); ++i)
        setData(index(row, i, QModelIndex()), QVariant(alignment), Qt::TextAlignmentRole);
}

bool ETableModel::isEmpty() { return maindata.isEmpty(); }

void ETableModel::SetColumnFormat(int column, int format)
{
    while (column >= ColFormat.size())
        ColFormat.append(NOCOLFORMAT);
    ColFormat.replace(column, format);
}

void ETableModel::SetHeaders(QStringList hdrl)
{
    hdr = hdrl;
    int colcount = hdr.size() - columnCount();
    insertColumns(columnCount(), colcount);
}

void ETableModel::AddRowWithData(QVector<QVariant> vl)
{
    int currow = rowCount();
    addRow();
    if (vl.size() > hdr.size()) // в переданном списке больше колонок, чем в модели
    {
        for (int i = hdr.size(); i < vl.size(); ++i)
            addColumn("");
    }
    for (int i = 0; i < vl.size(); ++i) // цикл по строкам
        setData(index(currow, i, QModelIndex()), vl.at(i), Qt::EditRole);
}

void ETableModel::setCellAttr(QModelIndex index, int fcset, int icon)
{
    if (icon != -1)
        maindata.at(index.row())->setIcon(index.column(), icons[icon]);
    maindata.at(index.row())->setColor(index.column(), colors[fcset]);
    maindata.at(index.row())->setFont(index.column(), fonts[fcset]);
}

void ETableModel::ClearModel()
{
    beginResetModel();
    if (!maindata.isEmpty())
        qDeleteAll(maindata.begin(), maindata.end());
    hdr.clear();
    maindata.clear();
    ColFormat.clear();
    endResetModel();
}
