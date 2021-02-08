#pragma once

#include <QColor>
#include <QFont>
#include <QIcon>
#include <QStringList>
#include <QVariant>

class ETableItemData
{
public:
    inline ETableItemData() : role(-1)
    {
    }
    inline ETableItemData(int r, const QVariant &v) : role(r), value(v)
    {
    }
    inline ETableItemData(const std::pair<const int &, const QVariant &> &p) : role(p.first), value(p.second)
    {
    }
    ~ETableItemData()
    {
    }
    int role;
    QVariant value;
    inline bool operator==(const ETableItemData &other) const
    {
        return role == other.role && value == other.value;
    }
    ETableItemData(const ETableItemData &) = default;
    ETableItemData(ETableItemData &&) = default;
    ETableItemData &operator=(const ETableItemData &) = default;
    ETableItemData &operator=(ETableItemData &&) = default;
};

using ETableItem = QList<ETableItemData>;

class ETableRow : public QObject
{
public:
    explicit ETableRow(QObject *parent = 0);

    void setData(int column, const QVariant &value, int role);
    QVariant data(int column, int role) const;

    void setColor(int column, const QColor &value);
    QColor color(int column) const;

    void setFont(int column, QFont font);
    QFont font(int column) const;

    void setIcon(int column, QIcon icon);
    QIcon icon(int column) const;

    void setTextAlignment(int column, int alignment = Qt::AlignLeft | Qt::AlignVCenter);
    int textAlignment(int column) const;

private:
    QList<ETableItem> values;
};

inline void ETableRow::setColor(int column, const QColor &value)
{
    setData(column, value, Qt::DecorationRole);
}

inline void ETableRow::setFont(int column, QFont font)
{
    setData(column, font, Qt::FontRole);
}

inline void ETableRow::setIcon(int column, QIcon icon)
{
    setData(column, icon, Qt::DecorationRole);
}

inline void ETableRow::setTextAlignment(int column, int alignment)
{
    setData(column, alignment, Qt::TextAlignmentRole);
}

inline QColor ETableRow::color(int column) const
{
    return qvariant_cast<QColor>(data(column, Qt::DecorationRole));
}

inline QFont ETableRow::font(int column) const
{
    return qvariant_cast<QFont>(data(column, Qt::FontRole));
}

inline QIcon ETableRow::icon(int column) const
{
    return qvariant_cast<QIcon>(data(column, Qt::DecorationRole));
}

inline int ETableRow::textAlignment(int column) const
{
    return qvariant_cast<int>(data(column, Qt::TextAlignmentRole));
}
