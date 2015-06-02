#include "s_tablemodel.h"

// ######################################## Переопределение методов QAbstractTableModel ####################################

s_tablemodel::s_tablemodel(QObject *parent) :
    QAbstractTableModel(parent)
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
    icons[0] = QIcon(":/res/hr.png");
    icons[1] = QIcon(":/res/ok.png");
    icons[2] = QIcon(":/res/cross.png");
    hdr.clear();
    maindata.clear();
}

s_tablemodel::~s_tablemodel()
{
}

QVariant s_tablemodel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole || role == Qt::EditRole) && (section < hdr.size()))
        return hdr.at(section);
    return QVariant();
}

bool s_tablemodel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}

QVariant s_tablemodel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        if ((index.row() < maindata.size()) && (index.column() < hdr.size()))
        {
            if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
                return maindata.at(index.row())->data(index.column());
            else if (role == Qt::FontRole)
                return QVariant::fromValue(QFont(maindata.at(index.row())->font(index.column())));
            else if (role == Qt::ForegroundRole)
                return QVariant::fromValue(QColor(maindata.at(index.row())->color(index.column())));
            else if (role == Qt::DecorationRole)
                return QVariant::fromValue(QIcon(maindata.at(index.row())->icon(index.column())));
        }
    }
    return QVariant();
}

// value должен представлять из себя запись вида: <value>.<links>, где links - вспомогательное поле, определяющее
// порядок работы с полем - подставляемый делегат, ссылку на списки и формат отображения

bool s_tablemodel::setData(const QModelIndex &index, QVariant &value, int role)
{
    if (index.isValid())
    {
        if (role == Qt::EditRole)
        {
            if (index.column() < hdr.size())
            {
                maindata.at(index.row())->setData(index.column(), value.toString()); // пишем само значение
                emit dataChanged(index, index);
                return true;
            }
        }
        else if (role == Qt::ForegroundRole)
        {
            maindata.at(index.row())->setColor(index.column(), value.value<QColor>());
            return true;
        }
        else if (role == Qt::FontRole)
        {
            maindata.at(index.row())->setFont(index.column(), value.value<QFont>());
            return true;
        }
    }
    return false;
}

Qt::ItemFlags s_tablemodel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
            return Qt::ItemIsEnabled;
    return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable;
}

QModelIndex s_tablemodel::index(int row, int column, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return createIndex(row, column);
}

bool s_tablemodel::insertColumns(int position, int columns, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertColumns(QModelIndex(), position, position+columns-1);
    if (!maindata.isEmpty()) // если в модели есть какие-то данные, то уже нельзя менять размерность таблицы по столбцам
        return false;
    for (int i = 0; i < columns; i++)
        hdr.insert(position, "");
    endInsertColumns();
    return true;
}

bool s_tablemodel::removeColumns(int position, int columns, const QModelIndex &index)
{
    beginRemoveColumns(index, position, position + columns - 1);
    if (!maindata.isEmpty())
        return false;
    if ((position+columns) > hdr.size())
        return false;
    for (int i = 0; i < columns; i++)
        hdr.removeAt(position);
    endRemoveColumns();
    return true;
}

bool s_tablemodel::insertRows(int position, int rows, const QModelIndex &index)
{
    int i, j;
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);
    for (i = 0; i < rows; i++)
    {
        s_tableitem *item = new s_tableitem();
        for (j = 0; j < hdr.size(); j++)
            item->setData(j, "");
        maindata.insert(position, item);
    }
    endInsertRows();
    return true;
}

bool s_tablemodel::removeRows(int position, int rows, const QModelIndex &index)
{
    beginRemoveRows(index, position, position + rows - 1);
    if ((position+rows) > maindata.size())
        return false;
    for (int i = 0; i < rows; i++)
    {
        s_tableitem *item = maindata.at(position);
        maindata.removeAt(position);
        delete item;
    }
    endRemoveRows();
    return true;
}

int s_tablemodel::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return hdr.size();
}

int s_tablemodel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return maindata.size();
}

// ###################################### Свои методы ############################################

int s_tablemodel::getHeaderPosition(QVariant hdrtext, Qt::Orientation orientation, int role)
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole || role == Qt::EditRole))
        return hdr.indexOf(hdrtext.toString());
    return -1;
}

void s_tablemodel::addColumn(const QString hdrtext)
{
    int lastEntry = hdr.size();
    insertColumns(lastEntry, 1, QModelIndex());
    hdr.replace(lastEntry, hdrtext);
}

void s_tablemodel::addRow()
{
    int lastEntry = maindata.size();
    insertRows(lastEntry, 1, QModelIndex());
}

void s_tablemodel::fillModel(QList<QStringList> lsl)
{
    int i;
    int j;
    QString vl;
    result = 0;
    if (lsl.size()>hdr.size()) // в переданном списке больше колонок, чем в модели
    {
        for (i = hdr.size(); i < lsl.size(); i++)
            addColumn("");
    }
    for (i = 0; i < lsl.at(0).size(); i++)  // цикл по строкам
    {
        if (i >= rowCount())
            addRow();
        for (j = 0; j < lsl.size(); j++) // цикл по столбцам
        {
            if (i > lsl.at(j).size()) // если строк в текущем столбце меньше, чем текущий номер строки, пишем пустое значение
                setData(index(i, j, QModelIndex()), QVariant(""), Qt::EditRole);
            else
            {
                setData(index(i, j, QModelIndex()), QVariant(lsl.at(j).at(i)), Qt::EditRole);
            }
        }
    }
}

// выдать значения по столбцу column в выходной QStringList

QStringList s_tablemodel::cvalues(int column)
{
    if (column > columnCount())
        return QStringList();
    result = 0;
    QStringList tmpsl;
    for (int row=0; row < rowCount(); row++)
        tmpsl.append(data(index(row,column,QModelIndex()),Qt::DisplayRole).toString());
    return tmpsl;
}

// выдать значения по строке row в выходной QStringList

QStringList s_tablemodel::rvalues(int row)
{
    if (row > rowCount())
        return QStringList();
    result = 0;
    QStringList tmpsl;
    for (int column = 0; column<columnCount(); column++)
        tmpsl.append(data(index(row,column,QModelIndex()),Qt::DisplayRole).toString());
    return tmpsl;
}

void s_tablemodel::setCellAttr(QModelIndex index, int fcset, int icon)
{
    if (icon != -1)
        maindata.at(index.row())->setIcon(index.column(), icons[icon]);
    maindata.at(index.row())->setColor(index.column(), colors[fcset]);
    maindata.at(index.row())->setFont(index.column(), fonts[fcset]);
}

void s_tablemodel::ClearModel()
{
    hdr.clear();
    maindata.clear();
}
