#include "etabwidget.h"

ETabWidget::ETabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setTabBar(new ETabBar());
    connect(this,SIGNAL(currentChanged(int)),this,SIGNAL(tabClicked(int)), Qt::ConnectionType(2));
}

void ETabWidget::TabClicked()
{
    int res = 0;
    Q_UNUSED(res);
}
