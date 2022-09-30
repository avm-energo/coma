#pragma once

#include "abstracttablerow.h"
#include "etablerow.h"

#include <QDebug>
#include <QObject>

class ErrorProtocolItem : public AbstractTableRow<std::vector<ETableItemData>>
{
    Q_OBJECT
public:
    explicit ErrorProtocolItem(QObject *parent = nullptr);
    void setData(int column, const QVariant &value, int role);
    QVariant data(int column, int role) const;
    friend QDebug operator<<(QDebug debug, const ErrorProtocolItem &item);
};

QDebug operator<<(QDebug debug, const ErrorProtocolItem &item)
{
#ifdef QT_GUI_LIB
    const auto &values = item.constData();
    for (const auto &i : *values)
        debug.nospace() << i.role << ":";
#endif
    return debug.maybeSpace();
}
