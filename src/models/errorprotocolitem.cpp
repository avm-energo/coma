#include "errorprotocolitem.h"

ErrorProtocolItem::ErrorProtocolItem(QObject *parent) : AbstractTableRow<std::vector<ETableItemData>>(parent)
{
}

void ErrorProtocolItem::setData(int column, const QVariant &value, int role)
{
    role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
    while (column >= values.size())
        values.push_back(ETableItemData(role, value));

    auto it = values.begin();
    while (it != values.end())
    {
        if ((*it).role == role)
        {

            if ((*it).value != value)
                (*it).value = value;
            return;
        }
        ++it;
    }
    values.push_back(ETableItemData(role, value));
}

QVariant ErrorProtocolItem::data(int column, int role) const
{
    role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
    auto it = values.cbegin();
    while (it != values.cend())
    {
        if ((*it).role == role)
        {
            if (role == Qt::DisplayRole)
            {
                return (*it).value.value<QStringList>().at(column);
            }
            return (*it).value;
        }
        ++it;
    }
    return QVariant();
}
