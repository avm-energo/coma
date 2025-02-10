#include <models/etablemodel.h>
#include <widgets/tvfunc.h>

TVFunc::TVFunc() { }

QString TVFunc::TVField(QWidget *parent, const QString &tvname, int column, bool isid)
{
    auto tv = parent->findChild<QTableView *>(tvname);
    if (tv == nullptr)
        return QString();
    auto tvModel = tv->model();
    auto tvIndex = tvModel->index(tv->currentIndex().row(), column);
    auto tmps = tvModel->data(tvIndex, Qt::DisplayRole).toString();
    // если поле с ИД, надо убрать первую цифру - номер таблицы и
    // разделяющую точку, если они присутствуют
    if (isid)
    {
        auto sl = tmps.split(".");
        if (sl.size() > 1) // есть номер таблицы
            tmps = sl.at(1);
        bool ok;
        int tmpi = tmps.toInt(&ok);
        if (!ok)
            return QString();
        tmps = QString::number(tmpi); // убираем старшие незначащие нули
    }
    return tmps;
}

void TVFunc::TVAutoResize(QWidget *parent, const QString &tvname)
{
    auto tv = parent->findChild<QTableView *>(tvname);
    if (tv == nullptr)
    {
        qDebug() << "Пустой tv";
        return;
    }
    tv->update();
    tv->repaint();
    tv->resizeColumnsToContents();
    tv->resizeRowsToContents();
}

QVariant TVFunc::TVData(QWidget *parent, const QString &tvname, int column)
{
    auto tv = parent->findChild<QTableView *>(tvname);
    if (tv == nullptr)
        return QVariant();
    auto m = static_cast<ETableModel *>(tv->model());
    if (m != nullptr)
        return m->index(tv->currentIndex().row(), column, QModelIndex()).data(Qt::DisplayRole);
    return QVariant();
}

QTableView *TVFunc::NewQTV(QWidget *parent, const QString &tvname, QAbstractItemModel *model)
{
    auto tv = new QTableView(parent);
    if (model != nullptr)
        tv->setModel(model);
    tv->setObjectName(tvname);
    tv->setSelectionBehavior(QAbstractItemView::SelectRows);
    tv->setSelectionMode(QAbstractItemView::SingleSelection);
    return tv;
}

void TVFunc::SetTVModel(QWidget *parent, const QString &tvname, QAbstractItemModel *model, bool sortenable)
{
    auto tv = parent->findChild<QTableView *>(tvname);
    if (tv == nullptr)
    {
        qDebug("Empty tv");
        return;
    }
    QItemSelectionModel *m = tv->selectionModel();
    tv->setModel(model);
    tv->resizeColumnsToContents();
    tv->setSortingEnabled(sortenable);
    delete m;
}

void TVFunc::SetQTVModel(QWidget *parent, const QString &tvname, QAbstractItemModel *model, bool sortenable)
{
    auto tv = parent->findChild<QTableView *>(tvname);
    if (tv == nullptr)
    {
        qDebug() << "Пустой tv";
        return;
    }
    auto selectModel = tv->selectionModel();
    tv->setModel(model);
    tv->resizeColumnsToContents();
    tv->setSortingEnabled(sortenable);
    delete selectModel;
}

void TVFunc::SortTV(QWidget *parent, const QString &tvname, int column, Qt::SortOrder sortorder)
{
    auto tv = parent->findChild<QTableView *>(tvname);
    if (tv == nullptr)
    {
        qDebug() << "Пустой tv";
        return;
    }
    if (column >= 0)
        tv->sortByColumn(column, sortorder);
}

QAbstractItemModel *TVFunc::TVModel(QWidget *parent, const QString &tvname)
{
    auto tv = parent->findChild<QTableView *>(tvname);
    if (tv == nullptr)
        return nullptr;
    return tv->model();
}
