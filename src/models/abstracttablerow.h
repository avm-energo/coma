#pragma once
#include <QColor>
#include <QFont>
#include <QIcon>
#include <QObject>
#include <QVariant>

template <typename C, typename T = typename C::value_type> class AbstractTableRow : public QObject
{
public:
    explicit AbstractTableRow(QObject *parent = 0) : QObject(parent)
    {
    }

    virtual void setData(int column, const QVariant &value, int role) = 0;
    //    {
    //        role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
    //        while (column >= values.size())
    //            values.push_back(ETableItem());

    //        auto it = values[column].begin();
    //        while (it != values.at(column).end())
    //        {
    //            if ((*it).role == role)
    //            {
    //                (*it).value = value;
    //                return;
    //            }
    //            ++it;
    //        }
    //        values[column].push_back(ETableItemData(role, value));
    //    }
    virtual QVariant data(int column, int role) const = 0;
    //    {
    //        role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
    //        auto it = values.at(column).cbegin();
    //        while (it != values.at(column).cend())
    //        {
    //            if ((*it).role == role)
    //                return (*it).value;
    //            ++it;
    //        }
    //        return QVariant();
    //    }

    void setColor(int column, const QColor &value)
    {
        setData(column, value, Qt::DecorationRole);
    }
    QColor color(int column) const
    {
        return qvariant_cast<QColor>(data(column, Qt::DecorationRole));
    }

    void setFont(int column, QFont font)
    {
        setData(column, font, Qt::FontRole);
    }
    QFont font(int column) const
    {
        return qvariant_cast<QFont>(data(column, Qt::FontRole));
    }

    void setIcon(int column, QIcon icon)
    {
        setData(column, icon, Qt::DecorationRole);
    }
    QIcon icon(int column) const
    {
        return qvariant_cast<QIcon>(data(column, Qt::DecorationRole));
    }

    void setTextAlignment(int column, int alignment = Qt::AlignLeft | Qt::AlignVCenter)
    {
        setData(column, alignment, Qt::TextAlignmentRole);
    }
    int textAlignment(int column) const
    {
        return qvariant_cast<int>(data(column, Qt::TextAlignmentRole));
    }

protected:
    C values;
};
