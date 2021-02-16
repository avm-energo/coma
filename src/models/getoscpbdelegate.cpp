#include "getoscpbdelegate.h"

#include "etablerow.h"
#include "etablemodel.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QStandardItem>
#include <QStandardItemModel>

GetOscPBDelegate::GetOscPBDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    _state = QStyle::State_Enabled;
}

void GetOscPBDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const ETableModel *model = static_cast<const ETableModel *>(index.model());
    QString text = model->data(index, Qt::DisplayRole).toString();
    QIcon icon = model->data(index, Qt::DecorationRole).value<QIcon>();
    QRect rect = option.rect;

    /*    if (!text.isEmpty())
        {
            QRect textRect(rect);
    //        textRect.setHeight(30);
            QFontMetrics fm(painter->font);
            int w = fm.width(text);
            int h = fm.height();
            float TextPosX = textRect.center().x()-w/2;
            float TextPosY = textRect.center().y()-h/2;
            QPointF TextPos(TextPosX, TextPosY);
            painter->drawText(TextPos,text);
        }
        else */
    if (!text.isEmpty() || !icon.isNull())
    {
        QRect buttonRect(rect);
        //    buttonRect.setY(textRect.y()+35);
        //    buttonRect.setHeight(30);
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

bool GetOscPBDelegate::editorEvent(
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
