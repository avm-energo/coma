#ifndef PUSHBUTTONDELEGATE_H
#define PUSHBUTTONDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

class PushButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    PushButtonDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(
        QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void clicked(const QModelIndex &index);

private:
    QStyle::State _state;

private slots:
};

#endif // PUSHBUTTONDELEGATE_H
