#ifndef GETOSCPBDELEGATE_H
#define GETOSCPBDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>

class GetOscPBDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GetOscPBDelegate(QObject *parent=0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void clicked(const QModelIndex &index);

private:
    QStyle::State _state;

private slots:
};

#endif // GETOSCPBDELEGATE_H
