#include "pushbuttondelegate.h"

#include "../models/etablemodel.h"
#include "../models/etablerow.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QStandardItem>
#include <QStandardItemModel>

PushButtonDelegate::PushButtonDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    _state = QStyle::State_Enabled;
}

void PushButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const ETableModel *model = static_cast<const ETableModel *>(index.model());
    QString text = model->data(index, Qt::DisplayRole).toString();
    QIcon icon = model->data(index, Qt::DecorationRole).value<QIcon>();
    QRect rect = option.rect;

    if (!text.isEmpty() || !icon.isNull())
    {
        QRect buttonRect(rect);

        QStyleOptionButton button;
        button.rect = buttonRect;
        if (!text.isEmpty())
            button.text = text;
        else if (!icon.isNull())
            button.icon = icon;
        button.state = _state | QStyle::State_Enabled;
        QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
    }
}

bool PushButtonDelegate::editorEvent(
    QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model);
    if ((event->type() == QEvent::MouseButtonPress) || (event->type() == QEvent::MouseButtonRelease))
        ;
    else
    {
        _state = QStyle::State_Raised;
        return true;
    }

    QRect buttonRect(option.rect);

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
