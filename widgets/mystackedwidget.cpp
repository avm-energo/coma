#include "mystackedwidget.h"

MyStackedWidget::MyStackedWidget (QWidget *parent) : QStackedWidget(parent)
{
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(onCurrentChanged(int)));
}

MyStackedWidget::~MyStackedWidget()
{

}

void MyStackedWidget::addWidget(QWidget *w)
{
    w->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QStackedWidget::addWidget(w);
}

void MyStackedWidget::onCurrentChanged(int index)
{
    QWidget *w = widget(index);
    Q_ASSERT(w);
    w->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    w->adjustSize();
    adjustSize();
}
