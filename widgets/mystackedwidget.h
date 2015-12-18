#ifndef MYSTACKEDWIDGET_H
#define MYSTACKEDWIDGET_H

#include <QObject>
#include <QStackedWidget>

class MyStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    MyStackedWidget(QWidget *parent=0);
    ~MyStackedWidget();

public:
    void addWidget(QWidget *w);

private slots:
    void onCurrentChanged(int index);
};

#endif // MYSTACKEDWIDGET_H
