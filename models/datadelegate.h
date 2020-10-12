#ifndef CHECKDELEGATE_H
#define CHECKDELEGATE_H

#include "../gen/error.h"
#include <QStyledItemDelegate>
#include <QWidget>

class DataDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    enum DataDelegateStyles
    {
        Label,
        LineEdit,
        SpinBox,
        ComboBox
    };

    enum DataOutputFormat
    {
        SIMPLE_CELL = 0, // general data output
        OUTVALUEINT = 1, // int
        OUTVALUEFLOAT0 = 2, // 0 digits after zero
        OUTVALUEFLOAT1 = 3,
        OUTVALUEFLOAT2 = 4,
        OUTVALUEFLOAT3 = 5,
        OUTVALUEFLOAT4 = 6,
        OUTVALUEFLOAT5 = 7, // 5 digits after zero
        OUTVALUESTRING = 8, // abracadabra
        OUTVALUEDOUBLE = 9 // 16e-018
    };

    DataDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    mutable DataDelegateStyles m_delegateType;
    mutable DataOutputFormat m_dataFormat;

    Error::Msg getStyleAndFormat(const QModelIndex &index) const;
};

#endif // CHECKDELEGATE_H
