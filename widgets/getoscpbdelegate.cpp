#include "getoscpbdelegate.h"
#include <QPainter>
#include <QApplication>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMouseEvent>
#include "s_tablemodel.h"
#include "s_tableitem.h"

GetOscPBDelegate::GetOscPBDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    _state = QStyle::State_Enabled;
}

void GetOscPBDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const s_tablemodel *model = static_cast<const s_tablemodel*>(index.model());
/*    s_tableitem *item = model->item(index.row());
    QString text = item->text(); */
    QString text = model->data(index, Qt::DisplayRole).toString();
    QRect rect = option.rect;

    QRect textRect(rect);
//    textRect.setHeight(30);
    painter->drawText(textRect,text);

    QRect buttonRect(rect);
//    buttonRect.setY(textRect.y()+35);
//    buttonRect.setHeight(30);
    QStyleOptionButton button;
    button.rect = buttonRect;
    button.text = text;
    button.state = _state | QStyle::State_Enabled;
    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool GetOscPBDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model);
    if ((event->type() == QEvent::MouseButtonPress) || (event->type() == QEvent::MouseButtonRelease));
    else
    {
        _state = QStyle::State_Raised;
        return true;
    }

    QRect buttonRect(option.rect);
//    buttonRect.setY(option.rect.y()+35);
//    buttonRect.setHeight(30);
    QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
    if (!buttonRect.contains(mEvent->pos()))
    {
        _state = QStyle::State_Raised;
        return true;
    }
    if (event->type() == QEvent::MouseButtonPress)
        _state = QStyle::State_Sunken;
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        _state = QStyle::State_Raised;
        emit clicked(index);
    }
    return true;
}
