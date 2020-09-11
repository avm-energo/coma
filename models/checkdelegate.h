#ifndef CHECKDELEGATE_H
#define CHECKDELEGATE_H

#include <QStyledItemDelegate>
#include <QWidget>

class CheckDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    enum CheckStyles
    {
        SIMPLE_CELL = 0,
        OUTVALUE = 1
    }

    explicit CheckDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // CHECKDELEGATE_H
