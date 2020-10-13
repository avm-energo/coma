#include "datadelegate.h"

#include "../gen/colors.h"
#include "../widgets/wd_func.h"
#include "valuemodel.h"

#include <QColor>
#include <QPainter>
#include <QVBoxLayout>

DataDelegate::DataDelegate(QObject *parent) : QStyledItemDelegate(parent) { }

void DataDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    painter->save();
    ValueItem *item = qvariant_cast<ValueItem *>(index.data(Qt::DisplayRole));
    if (item->format() != ValueItem::SIMPLE_CELL)
    {
        // ohh it's my column
        // better do something creative
        QString textToDisplay;
        switch (item->format())
        {
        case ValueItem::OUTVALUEINT:
            textToDisplay = QString::number(index.data().toInt());
            break;
        case ValueItem::OUTVALUEHEX:
            textToDisplay = QString::number(index.data().toUInt(), 16);
            break;
        case ValueItem::OUTVALUEFLOAT0:
        case ValueItem::OUTVALUEFLOAT1:
        case ValueItem::OUTVALUEFLOAT2:
        case ValueItem::OUTVALUEFLOAT3:
        case ValueItem::OUTVALUEFLOAT4:
        case ValueItem::OUTVALUEFLOAT5:
            textToDisplay = QString::number(index.data().toFloat(), 'g', item->format() - 2);
            break;
        case ValueItem::OUTVALUEDOUBLE:
            textToDisplay = QString::number(index.data().toDouble(), 'e', 4);
            break;
        case ValueItem::OUTVALUESTRING:
        default:
            textToDisplay = index.data().toString();
            break;
        }

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
        painter->drawText(rect, textToDisplay);
    }
    painter->restore();
}

QWidget *DataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    ValueItem *item = qvariant_cast<ValueItem *>(index.data(Qt::DisplayRole));
    //    if (getStyleAndFormat(index) != Error::Msg::NoError)
    //        return nullptr;
    QVBoxLayout *ml = new QVBoxLayout;
    ml->setContentsMargins(0, 0, 0, 0);
    QWidget *wdgt = new QWidget(parent);
    wdgt->setContentsMargins(0, 0, 0, 0);
    switch (item->style())
    {
    case ValueItem::DataStyles::ComboBox:
    {
        QStringList *list = static_cast<QStringList *>(item->addData());
        ml->addWidget(WDFunc::NewCB(parent, "", *list));
        break;
    }
    case ValueItem::DataStyles::Label:
        return nullptr;
    case ValueItem::DataStyles::LineEdit:
        ml->addWidget(WDFunc::NewLE(parent, ""));
        break;
    case ValueItem::DataStyles::SpinBox:
        SpbDataStyleAdditionalDataClass *spbadd = static_cast<SpbDataStyleAdditionalDataClass *>(item->addData());
        ml->addWidget(WDFunc::NewSPB(parent, "", spbadd->min, spbadd->max, spbadd->decimals));
        break;
    }
    wdgt->setLayout(ml);
    return wdgt;
}

void DataDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const { }

// Error::Msg DataDelegate::getStyleAndFormat(const QModelIndex &index) const
//{
//    QString links = index.data(ValueModel::DataFormatRole).toString();
//    QStringList tmpsl = links.split(".");
//    if (!tmpsl.size())
//        return Error::Msg::GeneralError;
//    m_delegateType = static_cast<ValueItem::DataStyles>(tmpsl.at(0).toInt());
//    tmpsl.removeAt(0);
//    if (!tmpsl.size())
//        return Error::Msg::GeneralError;
//    m_dataFormat = static_cast<ValueItem::DataFormat>(tmpsl.at(0).toInt());
//    return Error::Msg::NoError;
//}
