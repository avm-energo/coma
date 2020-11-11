#include "valuedelegate.h"

#include "../gen/colors.h"
#include "../widgets/wd_func.h"
#include "valuemodel.h"

#include <QColor>
#include <QPainter>
#include <QVBoxLayout>

ValueDelegate::ValueDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void ValueDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    painter->save();
    if (index.data(Qt::DisplayRole).canConvert<ValueItem *>())
    {
        ValueItem *item = qvariant_cast<ValueItem *>(index.data(Qt::DisplayRole));
        if (item->format() != ValueItem::SIMPLE_CELL)
        {
            // ohh it's my column
            // better do something creative
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
            painter->drawText(rect, convertItemDataToString(item));
        }
    }
    else
    {
        QRect rect = option.rect;
        painter->drawText(rect, index.data(Qt::DisplayRole).toString());
    }
    painter->restore();
}

QWidget *ValueDelegate::createEditor(
    QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
        if (item->addData().canConvert<QStringList *>())
        {
            QStringList *list = qvariant_cast<QStringList *>(item->addData());
            ml->addWidget(WDFunc::NewCB(parent, "cb", *list));
        }
        else
            return nullptr;
        break;
    }
    case ValueItem::DataStyles::Label:
        return nullptr;
    case ValueItem::DataStyles::LineEdit:
        ml->addWidget(WDFunc::NewLE(parent, "le"));
        break;
    case ValueItem::DataStyles::SpinBox:
        if (item->addData().canConvert<SpbAddDataStruct *>())
        {
            SpbAddDataStruct *spbadd = qvariant_cast<SpbAddDataStruct *>(item->addData());
            ml->addWidget(WDFunc::NewSPB(parent, "spb", spbadd->min, spbadd->max, spbadd->decimals));
        }
        else
            return nullptr;
        break;
    }
    wdgt->setLayout(ml);
    return wdgt;
}

void ValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    ValueItem *item = qvariant_cast<ValueItem *>(index.data(Qt::DisplayRole));
    switch (item->style())
    {
    case ValueItem::DataStyles::ComboBox:
    {
        if (item->data().canConvert<QString *>())
        {
            QString *data = qvariant_cast<QString *>(item->data());
            WDFunc::SetCBData(editor, "cb", *data);
        }
        break;
    }
    case ValueItem::DataStyles::LineEdit:
        WDFunc::SetLEData(editor, "le", convertItemDataToString(item));
        break;
    case ValueItem::DataStyles::SpinBox:
        WDFunc::SetSPBData(editor, "spb", convertItemDataToString(item).toDouble());
        break;
    default:
        break;
    }
}

void ValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    ValueItem *item = qvariant_cast<ValueItem *>(index.data(Qt::DisplayRole));
    QVariant var;
    switch (item->style())
    {
    case ValueItem::DataStyles::ComboBox:
    {
        if (item->data().canConvert<QString *>())
        {
            QString *data = qvariant_cast<QString *>(item->data());
            *data = WDFunc::CBData(editor, "cb");
        }
        break;
    }
    case ValueItem::DataStyles::LineEdit:
        switch (item->format())
        {
        case ValueItem::OUTVALUEINT:
        {
            if (item->data().canConvert<qint32 *>())
            {
                qint32 *data = qvariant_cast<qint32 *>(item->data());
                *data = WDFunc::LEData(editor, "le").toInt();
            }
            break;
        }
        case ValueItem::OUTVALUEHEX:
        {
            if (item->data().canConvert<quint32 *>())
            {
                quint32 *data = qvariant_cast<quint32 *>(item->data());
                *data = WDFunc::LEData(editor, "le").toUInt();
            }
            break;
        }
        case ValueItem::OUTVALUEFLOAT0:
        case ValueItem::OUTVALUEFLOAT1:
        case ValueItem::OUTVALUEFLOAT2:
        case ValueItem::OUTVALUEFLOAT3:
        case ValueItem::OUTVALUEFLOAT4:
        case ValueItem::OUTVALUEFLOAT5:
        {
            if (item->data().canConvert<float *>())
            {
                float *data = qvariant_cast<float *>(item->data());
                *data = WDFunc::LEData(editor, "le").toFloat();
            }
            break;
        }
        case ValueItem::OUTVALUEDOUBLE:
        {
            if (item->data().canConvert<double *>())
            {
                double *data = qvariant_cast<double *>(item->data());
                *data = WDFunc::LEData(editor, "le").toDouble();
            }
            break;
        }
        case ValueItem::OUTVALUESTRING:
        case ValueItem::SIMPLE_CELL:
        {
            if (item->data().canConvert<QString *>())
            {
                QString *data = qvariant_cast<QString *>(item->data());
                *data = WDFunc::LEData(editor, "le");
            }
            break;
        }
        default:
            break;
        }
    case ValueItem::DataStyles::SpinBox:
        if (item->addData().canConvert<double *>())
        {
            double *data = qvariant_cast<double *>(item->data());
            WDFunc::SPBData(editor, "spb", *data);
        }
        break;
    default:
        break;
    }
    var.setValue(item);
    model->setData(index, var, Qt::EditRole);
}

QString ValueDelegate::convertItemDataToString(ValueItem *vi) const
{
    switch (vi->format())
    {
    case ValueItem::OUTVALUEINT:
    {
        if (vi->data().canConvert<qint32 *>())
        {
            qint32 *data = qvariant_cast<qint32 *>(vi->data());
            return QString::number(*data);
        }
        break;
    }
    case ValueItem::OUTVALUEHEX:
    {
        if (vi->data().canConvert<qint32 *>())
        {
            quint32 *data = qvariant_cast<quint32 *>(vi->data());
            return QString::number(*data, 16);
        }
        break;
    }
    case ValueItem::OUTVALUEFLOAT0:
    case ValueItem::OUTVALUEFLOAT1:
    case ValueItem::OUTVALUEFLOAT2:
    case ValueItem::OUTVALUEFLOAT3:
    case ValueItem::OUTVALUEFLOAT4:
    case ValueItem::OUTVALUEFLOAT5:
    {
        if (vi->data().canConvert<float *>())
        {
            float *data = qvariant_cast<float *>(vi->data());
            return QString::number(*data, 'g', vi->format() - 2);
        }
        break;
    }
    case ValueItem::OUTVALUEDOUBLE:
    {
        if (vi->data().canConvert<double *>())
        {
            double *data = qvariant_cast<double *>(vi->data());
            return QString::number(*data, 'e', 4);
        }
        break;
    }
    case ValueItem::OUTVALUESTRING:
    default:
    {
        if (vi->data().canConvert<QString *>())
        {
            QString *data = qvariant_cast<QString *>(vi->data());
            return *data;
        }
        break;
    }
    }
    return QString();
}

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
