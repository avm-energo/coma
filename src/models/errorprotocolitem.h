#pragma once

#include "abstracttablerow.h"
#include "etablerow.h"

#include <QObject>
class ErrorProtocolItem : public AbstractTableRow<std::vector<ETableItemData>>
{
    Q_OBJECT
public:
    explicit ErrorProtocolItem(QObject *parent = nullptr);

signals:

    // AbstractTableRow interface
public:
    void setData(int column, const QVariant &value, int role);
    QVariant data(int column, int role) const;
};
