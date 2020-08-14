#ifndef ETABLEITEM_H
#define ETABLEITEM_H

#include <QColor>
#include <QFont>
#include <QIcon>
#include <QStringList>

class ETableItem
{
public:
    explicit ETableItem(ETableItem *parent = 0);

    QString data(int column) const;
    void setData(int column, const QString &value);
    void setColor(int column, QColor color);
    void setFont(int column, QFont font);
    void setIcon(int column, QIcon icon);
    void setTextAlignment(int column, int alignment = Qt::AlignLeft | Qt::AlignVCenter);
    QColor color(int column);
    QFont font(int column);
    QIcon icon(int column);
    int TextAlignment(int column);

private:
    QStringList itemData; // содержимое строки
    QList<QColor> itemColor;
    QList<QFont> itemFont;
    QList<QIcon> itemIcon;
    QList<int> itemTextAlignment;
};

#endif // ETABLEITEM_H
