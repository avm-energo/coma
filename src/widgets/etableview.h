#ifndef ETABLEVIEW_H
#define ETABLEVIEW_H

#include <QPaintEvent>
#include <QResizeEvent>
#include <QTableView>

class ETableView : public QTableView
{
    Q_OBJECT
public:
    explicit ETableView(QWidget *parent = 0);

public slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

protected:
    void resizeEvent(QResizeEvent *e);
};

#endif // ETABLEVIEW_H
