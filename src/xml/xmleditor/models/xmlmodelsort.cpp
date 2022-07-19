#include "xmlmodel.h"

decltype(auto) getComparator(Qt::SortOrder &order)
{
    return (order == Qt::AscendingOrder ? //
            [](QString &lhv, QString &rhv) { return (lhv > rhv); }
                                        : //
            [](QString &lhv, QString &rhv) { return (lhv < rhv); });
}

void swap(int row1, int row2, int col)
{
    ;
    ;
}

void XmlModel::sort(int column, Qt::SortOrder order)
{
    auto comparator = getComparator(order);
    int row = 0;
    foreach (auto &data, mHashTable)
    {
        auto str = data.value<QString>();

        row++;
    }
}
