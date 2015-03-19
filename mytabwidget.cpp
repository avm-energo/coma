#include "mytabwidget.h"

MyTabWidget::MyTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setTabBar(new TabBar());
}
