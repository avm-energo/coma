#include "etabwidget.h"

ETabWidget::ETabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setTabBar(new ETabBar());
    connect(this,SIGNAL(tabBarClicked(int)),this,SIGNAL(tabClicked(int)));
}

void ETabWidget::TabClicked()
{
    int res = 0;
    Q_UNUSED(res);
}
