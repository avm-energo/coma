#ifndef S_TQTABLEVIEW_H
#define S_TQTABLEVIEW_H

#include <QTableView>
#include <QPaintEvent>

class s_tqTableView : public QTableView
{
    Q_OBJECT
public:
//    explicit s_tqTableView(bool autoResize=false, QWidget *parent = 0);
    explicit s_tqTableView(QWidget *parent = 0);
    QVariant getAData();
    void setAData(QVariant dat);
//    QSize minimumSizeHint() const;
//    mutable bool datachangedintable;

signals:
    void datachanged();

public slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:
    QVariant adata;
//    bool autoResize;

protected:
//    void paintEvent(QPaintEvent *e);
};

#endif // S_TQTABLEVIEW_H
