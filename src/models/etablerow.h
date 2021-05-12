#pragma once

#include "abstracttablerow.h"

#include <QColor>
#include <QFont>
#include <QIcon>
#include <QStringList>
#include <QVarLengthArray>
#include <QVariant>
class ETableItemData
{
public:
    inline ETableItemData() : role(-1)
    {
    }
    inline ETableItemData(int r, const QVariant &v) : role(r), value(v)
    {
    }
    inline ETableItemData(const std::pair<const int &, const QVariant &> &p) : role(p.first), value(p.second)
    {
    }
    ~ETableItemData()
    {
    }
    int role;
    QVariant value;
    inline bool operator==(const ETableItemData &other) const
    {
        return role == other.role && value == other.value;
    }
    ETableItemData(const ETableItemData &) = default;
    ETableItemData(ETableItemData &&) = default;
    ETableItemData &operator=(const ETableItemData &) = default;
    ETableItemData &operator=(ETableItemData &&) = default;
};

using ETableItem = std::vector<ETableItemData>;
using ETableItemCollection = std::vector<ETableItem>;
// using ETableRow = AbstractTableRow<ETableItemCollection>;

class ETableRow : public AbstractTableRow<ETableItemCollection>
{
public:
    explicit ETableRow(QObject *parent = 0);
    void setData(int column, const QVariant &value, int role) override;
    QVariant data(int column, int role) const override;
};
