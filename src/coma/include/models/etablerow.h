#pragma once

#include <QColor>
#include <QDebug>
#include <QFont>
#include <QIcon>
#include <QStringList>
#include <QVarLengthArray>
#include <QVariant>
#include <models/abstracttablerow.h>

class ETableItemData
{
public:
    explicit ETableItemData();
    explicit ETableItemData(int r, const QVariant &v);
    explicit ETableItemData(const std::pair<const int &, const QVariant &> &p);
    ETableItemData(const ETableItemData &) = default;
    ETableItemData(ETableItemData &&) = default;
    ETableItemData &operator=(const ETableItemData &) = default;
    ETableItemData &operator=(ETableItemData &&) = default;
    ~ETableItemData() = default;
    bool operator==(const ETableItemData &other) const;
    friend QDebug operator<<(QDebug debug, const ETableItemData &item);

    int role;
    QVariant value;
};

using ETableItem = std::vector<ETableItemData>;
using ETableItemCollection = std::vector<ETableItem>;

class ETableRow : public AbstractTableRow<ETableItemCollection>
{
public:
    explicit ETableRow(QObject *parent = nullptr);
    void setData(int column, const QVariant &value, int role) override;
    QVariant data(int column, int role) const override;
};
