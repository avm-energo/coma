#pragma once

#include <QDebug>
#include <QObject>
#include <models/abstracttablerow.h>
#include <models/etablerow.h>

class ErrorProtocolItem : public AbstractTableRow<std::vector<ETableItemData>>
{
    Q_OBJECT
public:
    explicit ErrorProtocolItem(QObject *parent = nullptr);
    void setData(int column, const QVariant &value, int role);
    QVariant data(int column, int role) const;
    friend QDebug operator<<(QDebug debug, const ErrorProtocolItem &item);
};
