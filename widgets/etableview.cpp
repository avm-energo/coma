#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>
#include <QDialog>
#include "etableview.h"

ETableView::ETableView(QWidget *parent) :
    QTableView(parent)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setStyleSheet("QTableView {background-color: rgba(0,0,0,0);}");
    setFrameStyle(QFrame::NoFrame);
    setShowGrid(false);
    connect(this,SIGNAL(datachanged()), this, SLOT(resizeColumnsToContents()));
}

/*void ETableView::setAData(QVariant dat)
{
    this->adata = dat;
}

QVariant ETableView::getAData()
{
    return this->adata;
} */

void ETableView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    Q_UNUSED(roles);
    resizeColumnsToContents();
    emit datachanged();
}
