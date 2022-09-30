#include "etablerow.h"

#include <QDebug>

ETableItemData::ETableItemData() : role(-1)
{
}

ETableItemData::ETableItemData(int r, const QVariant &v) : role(r), value(v)
{
}

ETableItemData::ETableItemData(const std::pair<const int &, const QVariant &> &p) : role(p.first), value(p.second)
{
}

bool ETableItemData::operator==(const ETableItemData &other) const
{
    return role == other.role && value == other.value;
}

QDebug operator<<(QDebug debug, const ETableItemData &item)
{
#ifdef QT_GUI_LIB
    debug.nospace() << item.value << ":" << item.role;
#endif
    return debug.maybeSpace();
}

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
