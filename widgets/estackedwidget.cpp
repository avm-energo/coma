#include "estackedwidget.h"

EStackedWidget::EStackedWidget (QWidget *parent) : QStackedWidget(parent)
{
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(onCurrentChanged(int)));
}

EStackedWidget::~EStackedWidget()
{

}

void EStackedWidget::addWidget(QWidget *w)
{
    w->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QStackedWidget::addWidget(w);
}

void EStackedWidget::onCurrentChanged(int index)
{
    QWidget *w = widget(index);
    Q_ASSERT(w);
    w->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    w->adjustSize();
    adjustSize();
}
