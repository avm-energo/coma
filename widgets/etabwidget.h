#ifndef ETABWIDGET_H
#define ETABWIDGET_H

#include "../gen/colors.h"

#include <QIcon>
#include <QPainter>
#include <QString>
#include <QStyleOptionTab>
#include <QStylePainter>
#include <QTabBar>
#include <QTabWidget>

class ETabBar : public QTabBar
{
public:
    explicit ETabBar(QWidget *parent = 0);

protected:
    QSize tabSizeHint(int index) const;
    void paintEvent(QPaintEvent *);
};

class ETabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit ETabWidget(QWidget *parent = nullptr);

signals:
    void tabClicked(int index);

public slots:

private slots:
    void TabClicked();
};

#endif // ETABWIDGET_H
