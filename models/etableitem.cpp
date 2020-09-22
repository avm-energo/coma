#include "etableitem.h"

#include <QDebug>

ETableItem::ETableItem(QObject *parent) : QObject(parent)
{
}

QString ETableItem::data(int column) const
{
    if (!itemData.isEmpty() && column < itemData.size())
        return itemData.at(column);
    else
        return QString();
}

void ETableItem::setData(int column, const QString &data)
{
    //    itemData.reserve(column + 1);
    //    itemData.insert(column, data);
    // itemData.push_back(data);
    while (column >= itemData.size())
        itemData.append(QString());
    itemData.replace(column, data);
}

void ETableItem::appendData(const QString &value)
{
    itemData.append(value);
}

void ETableItem::setColor(int column, QColor color)
{
    itemColor.reserve(column + 1);
    itemColor.insert(column, color);
    //    while (column >= itemColor.size())
    //        itemColor.append(QColor());
    //    itemColor.replace(column, color);
}

void ETableItem::setFont(int column, QFont font)
{
    itemFont.reserve(column + 1);
    itemFont.insert(column, font);
    //    while (column >= itemFont.size())
    //        itemFont.append(QFont());
    //    itemFont.replace(column, font);
}

void ETableItem::setIcon(int column, QIcon icon)
{
    itemIcon.reserve(column + 1);
    itemIcon.insert(column, icon);
    //    while (column >= itemIcon.size())
    //        itemIcon.append(QIcon());
    //    itemIcon.replace(column, icon);
}

void ETableItem::setTextAlignment(int column, int alignment)
{
    itemTextAlignment.reserve(column + 1);
    itemTextAlignment.insert(column, alignment);
    //    while (column >= itemTextAlignment.size())
    //        itemTextAlignment.append(Qt::AlignLeft | Qt::AlignVCenter);
    //    itemTextAlignment.replace(column, alignment);
}

QColor ETableItem::color(int column)
{
    if (!itemColor.isEmpty() && column < itemColor.size())
        return itemColor.at(column);
    else
        return QColor();
}

QFont ETableItem::font(int column)
{
    if (!itemFont.isEmpty() && column < itemFont.size())
        return itemFont.at(column);
    else
        return QFont();
}

QIcon ETableItem::icon(int column)
{
    if (!itemIcon.isEmpty() && column < itemIcon.size())
        return itemIcon.at(column);
    else
        return QIcon();
}

int ETableItem::TextAlignment(int column)
{
    if (!itemTextAlignment.isEmpty() && column < itemTextAlignment.size())
        return itemTextAlignment.at(column);
    else
        return (Qt::AlignLeft | Qt::AlignVCenter);
}

QVariant ETableItem::uData(int column) const
{
    if (!itemUData.isEmpty() && column < itemUData.size())
        return itemUData.at(column);
    else
        return QVariant();
}

void ETableItem::setUData(int column, QVariant data)
{
    itemUData.reserve(column + 1);
    itemUData.insert(column, data);
}
