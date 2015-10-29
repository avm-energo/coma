#include "mytabwidget.h"

MyTabWidget::MyTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setTabBar(new TabBar());
    connect(this,SIGNAL(tabBarClicked(int)),this,SLOT(TabClicked()));
}

void MyTabWidget::TabClicked()
{
    int res = 0;
    Q_UNUSED(res);
}
