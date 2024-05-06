#pragma once

#include <QColor>
#include <QFont>
#include <QIcon>
#include <QObject>
#include <QVariant>

template <typename C, typename T = typename C::value_type> //
class AbstractTableRow : public QObject
{
public:
    explicit AbstractTableRow(QObject *parent = 0) : QObject(parent)
    {
    }

    virtual void setData(int column, const QVariant &value, int role) = 0;

    virtual QVariant data(int column, int role) const = 0;

    auto size() const
    {
        return values.size();
    }

    const C *constData() const
    {
        return &values;
    }

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
