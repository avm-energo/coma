#include "datadelegate.h"

#include "../gen/colors.h"

#include <QColor>
#include <QPainter>
#include <QVBoxLayout>

DataDelegate::DataDelegate(QObject *parent) : QStyledItemDelegate(parent) { }

void DataDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.data(Qt::UserRole) == OUTVALUEINT)
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

QWidget *DataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    if (getStyleAndFormat(index) != Error::Msg::NoError)
        return nullptr;
    QVBoxLayout *ml = new QVBoxLayout;
    ml->setContentsMargins(0, 0, 0, 0);
    QWidget *wdgt = new QWidget(parent);
    wdgt->setContentsMargins(0, 0, 0, 0);
    switch (m_delegateType)
    {
    case DataDelegateStyles::ComboBox:
    {
        ml->addWidget(WDFunc) break;
    }
    case DataDelegateStyles::Label:
    {
        break;
    }
    }
}

Error::Msg DataDelegate::getStyleAndFormat(const QModelIndex &index) const
{
    QString links = index.data(Qt::UserRole).toString();
    QStringList tmpsl = links.split(".");
    if (!tmpsl.size())
        return Error::Msg::GeneralError;
    m_delegateType = static_cast<DataDelegateStyles>(tmpsl.at(0).toInt());
    tmpsl.removeAt(0);
    if (!tmpsl.size())
        return Error::Msg::GeneralError;
    m_dataFormat = static_cast<DataOutputFormat>(tmpsl.at(0).toInt());
    return Error::Msg::NoError;
}
