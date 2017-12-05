#include "etableitem.h"

ETableItem::ETableItem(ETableItem *parent)
{
    Q_UNUSED(parent);
}

QString ETableItem::data(int column) const
{
    if (column < itemData.size())
        return itemData.at(column);
    else
        return QString();
}

void ETableItem::setData(int column, const QString &data)
{
    while (column >= itemData.size())
        itemData.append(QString());
    itemData.replace(column, data);
}

void ETableItem::setColor(int column, QColor color)
{
    while (column >= itemColor.size())
        itemColor.append(QColor());
    itemColor.replace(column, color);
}

void ETableItem::setFont(int column, QFont font)
{
    while (column >= itemFont.size())
        itemFont.append(QFont());
    itemFont.replace(column, font);
}

void ETableItem::setIcon(int column, QIcon icon)
{
    while (column >= itemIcon.size())
        itemIcon.append(QIcon());
    itemIcon.replace(column, icon);
}

void ETableItem::setTextAlignment(int column, int alignment)
{
    while (column >= itemTextAlignment.size())
        itemTextAlignment.append(Qt::AlignLeft | Qt::AlignVCenter);
    itemTextAlignment.replace(column, alignment);
}

QColor ETableItem::color(int column)
{
    if (column < itemColor.size())
        return itemColor.at(column);
    else
        return QColor();
}

QFont ETableItem::font(int column)
{
    if (column < itemFont.size())
        return itemFont.at(column);
    else
        return QFont();
}

QIcon ETableItem::icon(int column)
{
    if (column < itemIcon.size())
        return itemIcon.at(column);
    else
        return QIcon();
}

int ETableItem::TextAlignment(int column)
{
    if (column < itemTextAlignment.size())
        return itemTextAlignment.at(column);
    else
        return (Qt::AlignLeft | Qt::AlignVCenter);
}
