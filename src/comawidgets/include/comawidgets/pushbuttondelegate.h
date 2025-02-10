#pragma once

#include <QObject>
#include <QStyledItemDelegate>

class PushButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit PushButtonDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(
        QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void clicked(const QModelIndex &index);

private:
    QStyle::State _state;

private slots:
};
