#include "s_tqtableview.h"
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>
#include <QDialog>

s_tqTableView::s_tqTableView(QWidget *parent) :
    QTableView(parent)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setStyleSheet("QTableView {background-color: rgba(0,0,0,0);}");
    setFrameStyle(QFrame::NoFrame);
    setShowGrid(false);
    connect(this,SIGNAL(datachanged()), this, SLOT(resizeColumnsToContents()));
}

void s_tqTableView::setAData(QVariant dat)
{
    this->adata = dat;
}

QVariant s_tqTableView::getAData()
{
    return this->adata;
}

void s_tqTableView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    Q_UNUSED(roles);
        resizeColumnsToContents();
    emit datachanged();
}
