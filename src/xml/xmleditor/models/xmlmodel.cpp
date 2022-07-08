#include "xmlmodel.h"

const std::map<QString, GroupTypes> XmlModel::types {
    { "resources", GroupTypes::Resources }, //
    { "check", GroupTypes::Check },         //
    { "groups", GroupTypes::Groups },       //
    { "signals", GroupTypes::Signals },     //
    { "records", GroupTypes::Records }      //
};

const std::map<GroupTypes, QStringList> XmlModel::settings {
    { GroupTypes::Resources, { "XML", "Описание" } }, //
    { GroupTypes::Check, { "XML", "Описание" } },     //
    { GroupTypes::Groups, { "" } },                   //
    { GroupTypes::Signals, { "" } },                  //
    { GroupTypes::Records, { "" } }                   //
};

XmlModel::XmlModel(int rows, int cols, QObject *parent) : QAbstractTableModel(parent), mRows(rows), mCols(cols)
{
}

QVariant XmlModel::data(const QModelIndex &index, int nRole) const
{
    if (index.isValid())
    {
        switch (nRole)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case (Qt::UserRole + 1):
            return mHashTable.value(index, QString(""));
        default:
            break;
        }
    }
    return QVariant();
}

bool XmlModel::setData(const QModelIndex &index, const QVariant &val, int nRole)
{
    if (index.isValid() && val.isValid())
    {
        if ((index.column() < mCols) && (index.row() < mRows))
        {
            if (nRole == Qt::EditRole || nRole == (Qt::UserRole + 1))
            {
                mHashTable.insert(index, val);
                emit dataChanged(index, index);
            }
        }
    }
    return false;
}

int XmlModel::rowCount([[maybe_unused]] const QModelIndex &index) const
{
    return mRows;
}

int XmlModel::columnCount([[maybe_unused]] const QModelIndex &index) const
{
    return mCols;
}

Qt::ItemFlags XmlModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    return index.isValid() ? (flags | Qt::ItemIsEditable) : flags;
}

void XmlModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    int column = 0;
    for (auto &label : labels)
    {
        setHeaderData(column, Qt::Horizontal, label);
        column++;
    }
}
