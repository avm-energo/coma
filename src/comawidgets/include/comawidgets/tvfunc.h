#pragma once

#include <QTableView>

class TVFunc
{
public:
    TVFunc();

    [[nodiscard]] static QString TVField(QWidget *parent, const QString &tvname, int column, bool isid = false);
    static void TVAutoResize(QWidget *parent, const QString &tvname);
    static void SetTVModel(QWidget *parent, const QString &tvname, QAbstractItemModel *model, bool sortenable = false);
    static void SetQTVModel(QWidget *parent, const QString &tvname, QAbstractItemModel *model, bool sortenable = false);
    [[nodiscard]] static QTableView *NewQTV(QWidget *parent, const QString &tvname, QAbstractItemModel *model);
    [[nodiscard]] static QAbstractItemModel *TVModel(QWidget *parent, const QString &tvname);

    static void SortTV(QWidget *parent, const QString &tvname, int column, Qt::SortOrder sortorder);
    [[nodiscard]] static QVariant TVData(QWidget *parent, const QString &tvname, int column);
};
