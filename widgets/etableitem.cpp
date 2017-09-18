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
    if (column < itemData.size())
        itemData.replace(column, data);
    else
        itemData.append(data);
}

void ETableItem::setColor(int column, QColor color)
{
    if (column < itemColor.size())
        itemColor.replace(column, color);
    else
        itemColor.append(color);
}

void ETableItem::setFont(int column, QFont font)
{
    if (column < itemFont.size())
        itemFont.replace(column, font);
    else
        itemFont.append(font);
}

void ETableItem::setIcon(int column, QIcon icon)
{
    if (column < itemIcon.size())
        itemIcon.replace(column, icon);
    else
        itemIcon.append(icon);
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
