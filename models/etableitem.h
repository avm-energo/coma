#ifndef ETABLEITEM_H
#define ETABLEITEM_H

#include <QColor>
#include <QFont>
#include <QIcon>
#include <QStringList>
#include <QVariant>

class ETableItem : public QObject
{
public:
    explicit ETableItem(QObject *parent = 0);

    QString data(int column) const;
    void setData(int column, const QString &value);
    QVariant uData(int column) const;
    void setUData(int column, QVariant data);
    QObject *aData(int column) const;
    void setAData(int column, QObject *data);
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
    QStringList itemData; // содержимое строки
    QList<QVariant> itemUData; // содержимое по DataFormatRole
    QList<QColor> itemColor;
    QList<QFont> itemFont;
    QList<QIcon> itemIcon;
    QList<int> itemTextAlignment;
    QList<QObject *> itemAData; // содержимое по AdditionalDataRole
};

#endif // ETABLEITEM_H
