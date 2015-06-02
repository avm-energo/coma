#ifndef s_tableitem_H
#define s_tableitem_H

#include <QStringList>
#include <QColor>
#include <QFont>
#include <QIcon>

class s_tableitem
{
public:
    explicit s_tableitem(s_tableitem *parent = 0);

    QString data(int column) const;
    void setData(int column, const QString &value);
    void setColor(int column, QColor color);
    void setFont(int column, QFont font);
    void setIcon(int column, QIcon icon);
    QColor color(int column);
    QFont font(int column);
    QIcon icon(int column);

private:
    QStringList itemData; // содержимое строки
    QList<QColor> itemColor;
    QList<QFont> itemFont;
    QList<QIcon> itemIcon;
};

#endif // s_tableitem_H
