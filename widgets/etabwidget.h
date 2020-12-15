#ifndef ETABWIDGET_H
#define ETABWIDGET_H

#include <QPaintEvent>
#include <QTabBar>
#include <QTabWidget>
class ETabBar : public QTabBar
{
public:
    explicit ETabBar(QWidget *parent = nullptr);

protected:
    QSize tabSizeHint(int index) const;
    void paintEvent(QPaintEvent *);
};

class ETabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit ETabWidget(QWidget *parent = nullptr);
};

#endif // ETABWIDGET_H
