#ifndef ETABLEVIEW_H
#define ETABLEVIEW_H

#include <QTableView>
#include <QPaintEvent>

class ETableView : public QTableView
{
    Q_OBJECT
public:
//    explicit ETableView(bool autoResize=false, QWidget *parent = 0);
    explicit ETableView(QWidget *parent = 0);
//    QVariant getAData();
//    void setAData(QVariant dat);
//    QSize minimumSizeHint() const;
//    mutable bool datachangedintable;

signals:
    void datachanged();

public slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:
//    QVariant adata;
//    bool autoResize;

protected:
//    void paintEvent(QPaintEvent *e);
};

#endif // ETABLEVIEW_H
