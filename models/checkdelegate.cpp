#include "checkdelegate.h"

#include "../gen/colors.h"

#include <QColor>
#include <QPainter>

CheckDelegate::CheckDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void CheckDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.data(Qt::UserRole) == OUTVALUE)
    {
        // ohh it's my column
        // better do something creative
        painter->save();
        QRect rect = option.rect;
        QBrush brush;
        brush.setColor(QColor(Colors::ACONFOCLR));
        painter->setPen(Qt::SolidLine);
        painter->setBrush(brush);
        if (option.state & QStyle::State_Selected)
            painter->setPen(QColor(Qt::darkGreen));
        else
            painter->setPen(QColor(Qt::lightGray));
        painter->drawRoundedRect(rect, 3, 3);
        painter->restore();
    }
    else // it's just a common column. Live it in default way
        QStyledItemDelegate::paint(painter, option, index);
}
