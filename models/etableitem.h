#ifndef ETABLEITEM_H
#define ETABLEITEM_H

#include <QColor>
#include <QFont>
#include <QIcon>
#include <QStringList>
#include <QVariant>

class ETableItem
{
public:
    explicit ETableItem(ETableItem *parent = 0);

    QString data(int column) const;
    void setData(int column, const QString &value);
    template <typename T> T uData(int column) const;
    template <typename T> void setUData(int column, T data);
    void appendData(const QString &value);
    void setColor(int column, QColor color);
    void setFont(int column, QFont font);
    void setIcon(int column, QIcon icon);
    void setTextAlignment(int column, int alignment = Qt::AlignLeft | Qt::AlignVCenter);
    QColor color(int column);
    QFont font(int column);
    QIcon icon(int column);
    int TextAlignment(int column);

private:
    QStringList itemData;      // содержимое строки
    QList<QVariant> itemUData; // содержимое по UserRole
    QList<QColor> itemColor;
    QList<QFont> itemFont;
    QList<QIcon> itemIcon;
    QList<int> itemTextAlignment;
};

template <typename T> void ETableItem::setUData(int column, T data)
{
    itemUData.reserve(column + 1);
    itemUData.insert(column, data);
}

template <typename T> T ETableItem::uData(int column) const
{
    if (!itemUData.isEmpty() && column < itemUData.size())
        return itemUData.at(column);
    else
        return QVariant();
}

#endif // ETABLEITEM_H
