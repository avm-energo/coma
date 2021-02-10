#include "etablerow.h"

#include <QDebug>

// ETableRow::ETableRow(QObject *parent) : QObject(parent)
//{
//}

// QVariant ETableRow::data(int column, int role) const
//{
//    role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
//    ETableItem::const_iterator it = values.at(column).cbegin();
//    while (it != values.at(column).cend())
//    {
//        if ((*it).role == role)
//            return (*it).value;
//        ++it;
//    }
//    return QVariant();
//}

// void ETableRow::setData(int column, const QVariant &value, int role)
//{
//    role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
//    while (column >= values.size())
//        values.push_back(ETableItem());

//    ETableItem::iterator it = values[column].begin();
//    while (it != values.at(column).end())
//    {
//        if ((*it).role == role)
//        {
//            (*it).value = value;
//            return;
//        }
//        ++it;
//    }
//    values[column].push_back(ETableItemData(role, value));
//}

ETableRow::ETableRow(QObject *parent) : AbstractTableRow<ETableItemCollection>(parent)
{
}

void ETableRow::setData(int column, const QVariant &value, int role)
{
    role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
    while (column >= values.size())
        values.push_back(ETableItem());

    auto it = values[column].begin();
    while (it != values.at(column).end())
    {
        if ((*it).role == role)
        {
            (*it).value = value;
            return;
        }
        ++it;
    }
    values[column].push_back(ETableItemData(role, value));
}

QVariant ETableRow::data(int column, int role) const
{
    role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
    auto it = values.at(column).cbegin();
    while (it != values.at(column).cend())
    {
        if ((*it).role == role)
            return (*it).value;
        ++it;
    }
    return QVariant();
}
