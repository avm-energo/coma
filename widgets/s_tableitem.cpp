#include "s_tableitem.h"

s_tableitem::s_tableitem(s_tableitem *parent)
{
    Q_UNUSED(parent);
}

QString s_tableitem::data(int column) const
{
    if (column < itemData.size())
        return itemData.at(column);
    else
        return QString();
}

void s_tableitem::setData(int column, const QString &data)
{
    if (column < itemData.size())
        itemData.replace(column, data);
    else
        itemData.append(data);
}

void s_tableitem::setColor(int column, QColor color)
{
    if (column < itemColor.size())
        itemColor.replace(column, color);
    else
        itemColor.append(color);
}

void s_tableitem::setFont(int column, QFont font)
{
    if (column < itemFont.size())
        itemFont.replace(column, font);
    else
        itemFont.append(font);
}

void s_tableitem::setIcon(int column, QIcon icon)
{
    if (column < itemIcon.size())
        itemIcon.replace(column, icon);
    else
        itemIcon.append(icon);
}

QColor s_tableitem::color(int column)
{
    if (column < itemColor.size())
        return itemColor.at(column);
    else
        return QColor();
}

QFont s_tableitem::font(int column)
{
    if (column < itemFont.size())
        return itemFont.at(column);
    else
        return QFont();
}

QIcon s_tableitem::icon(int column)
{
    if (column < itemIcon.size())
        return itemIcon.at(column);
    else
        return QIcon();
}
