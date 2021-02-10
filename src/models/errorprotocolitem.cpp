#include "errorprotocolitem.h"

ErrorProtocolItem::ErrorProtocolItem(QObject *parent) : AbstractTableRow<std::vector<ETableItemData *>>(parent)
{
}

void ErrorProtocolItem::setData(int column, const QVariant &value, int role)
{
    role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
    // while (column >= values.size())
    //     values.push_back((new ETableItemData()));

    auto it = values.begin();
    while (it != values.end())
    {
        if ((*it)->role == role)
        {
            // property for row, value for row item
            if (role == Qt::DisplayRole)
            {
                Q_ASSERT((*it)->value.canConvert<QStringList>());
                auto strList = static_cast<QStringList *>((*it)->value.data());
                strList->insert(column, value.toString());
                return;
            }
            (*it)->value = value;
            return;
        }
        ++it;
    }
}

QVariant ErrorProtocolItem::data(int column, int role) const
{
    {
        role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
        auto it = values.cbegin();
        while (it != values.cend())
        {
            if ((*it)->role == role)
                if (role == Qt::DisplayRole)
                {
                    (*it)->value.value<QStringList>().at(column);
                }
            return (*it)->value;
            ++it;
        }
        return QVariant();
    }
    return QVariant();
}
