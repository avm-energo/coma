#ifndef ESTACKEDWIDGET_H
#define ESTACKEDWIDGET_H

#include <QStackedWidget>

class EStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    EStackedWidget(QWidget *parent = 0);
    ~EStackedWidget();

public:
    void addWidget(QWidget *w);

private slots:
    void onCurrentChanged(int index);
};

#endif // ESTACKEDWIDGET_H
