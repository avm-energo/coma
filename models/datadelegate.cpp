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
    DataBlock::DataFormat dataFormat = static_cast<DataBlock::DataFormat>(index.data(Qt::UserRole).toInt());
    if (dataFormat != DataBlock::SIMPLE_CELL)
    {
        // ohh it's my column
        // better do something creative
        QString textToDisplay;
        switch (dataFormat)
        {
        case DataBlock::OUTVALUEINT:
            textToDisplay = QString::number(index.data().toInt());
            break;
        case DataBlock::OUTVALUEHEX:
            textToDisplay = QString::number(index.data().toUInt(), 16);
            break;
        case DataBlock::OUTVALUEFLOAT0:
        case DataBlock::OUTVALUEFLOAT1:
        case DataBlock::OUTVALUEFLOAT2:
        case DataBlock::OUTVALUEFLOAT3:
        case DataBlock::OUTVALUEFLOAT4:
        case DataBlock::OUTVALUEFLOAT5:
            textToDisplay = QString::number(index.data().toFloat(), 'g', dataFormat - 2);
            break;
        case DataBlock::OUTVALUEDOUBLE:
            textToDisplay = QString::number(index.data().toDouble(), 'e', 4);
            break;
        case DataBlock::OUTVALUESTRING:
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
    if (getStyleAndFormat(index) != Error::Msg::NoError)
        return nullptr;
    QVBoxLayout *ml = new QVBoxLayout;
    ml->setContentsMargins(0, 0, 0, 0);
    QWidget *wdgt = new QWidget(parent);
    wdgt->setContentsMargins(0, 0, 0, 0);
    switch (m_delegateType)
    {
    case DataBlock::DataStyles::ComboBox:
    {
        QStringList *list = qvariant_cast<QStringList *>(index.data(ValueModel::AdditionalDataRole));
        ml->addWidget(WDFunc::NewCB(parent, "", *list));
        break;
    }
    case DataBlock::DataStyles::Label:
        return nullptr;
    case DataBlock::DataStyles::LineEdit:
        ml->addWidget(WDFunc::NewLE(parent, ""));
        break;
    case DataBlock::DataStyles::SpinBox:
        DataBlock::SpbDataStyleAdditionalDataClass *spbadd = qvariant_cast<DataBlock::SpbDataStyleAdditionalDataClass *>(index.data(ValueModel::AdditionalDataRole));
        ml->addWidget(WDFunc::NewSPB(parent, "", spbadd->min, spbadd->max, spbadd->decimals));
        break;
    }
    wdgt->setLayout(ml);
    return wdgt;
}

void DataDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

}

Error::Msg DataDelegate::getStyleAndFormat(const QModelIndex &index) const
{
    QString links = index.data(ValueModel::DataFormatRole).toString();
    QStringList tmpsl = links.split(".");
    if (!tmpsl.size())
        return Error::Msg::GeneralError;
    m_delegateType = static_cast<DataBlock::DataStyles>(tmpsl.at(0).toInt());
    tmpsl.removeAt(0);
    if (!tmpsl.size())
        return Error::Msg::GeneralError;
    m_dataFormat = static_cast<DataBlock::DataFormat>(tmpsl.at(0).toInt());
    return Error::Msg::NoError;
}
