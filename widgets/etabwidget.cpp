#include "etabwidget.h"

ETabWidget::ETabWidget(QWidget *parent) : QTabWidget(parent)
{
    setTabBar(new ETabBar);
    setTabPosition(QTabWidget::West);
    connect(this, SIGNAL(tabBarClicked(int)), this, SIGNAL(tabClicked(int)));
}

void ETabWidget::TabClicked()
{
    int res = 0;
    Q_UNUSED(res);
}

ETabBar::ETabBar(QWidget *parent) : QTabBar(parent)
{
    setIconSize(QSize(18, 18));
    QString tbss = "QTabBar::tab {background-color: " + QString(Colors::ACONFGCLR)
        + "border: 0px solid;"
          "color: #000000;}"
          "QTabBar::tab::selected {background-color: "
        + QString(Colors::NFTCOLOR) + ";}";
    this->setStyleSheet(tbss);
}
