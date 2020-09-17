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
    Q_UNUSED(res)
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

QSize ETabBar::tabSizeHint(int index) const
{
    Q_UNUSED(index)
    return QSize(150, 40);
    //        QSize s = QTabBar::tabSizeHint(index);
    //        s.transpose();
    //        return s;
}

void ETabBar::paintEvent(QPaintEvent *)
{
    /*        QStylePainter p(this);
                for (int index = 0; index < count(); index++)
                {
                    QStyleOptionTab tab;
                    initStyleOption(&tab, index);
                    QIcon tempIcon = tab.icon;
                    QString tempText = tab.text;
                    tab.icon = QIcon();
                    tab.text = QString();
                    p.drawControl(QStyle::CE_TabBarTab, tab);
                    QPainter painter;
                    painter.begin(this);
                    QRect tabrect = tabRect(index);
                    tabrect.adjust(0, 8, 0, -8);
                    painter.drawText(tabrect, Qt::AlignVCenter | Qt::AlignHCenter, tempText);
                    tempIcon.paint(&painter, 0, tabrect.top(), tab.iconSize.width(), tab.iconSize.height(), Qt::AlignTop
           | Qt::AlignHCenter); painter.end();
                } */

    QStylePainter painter(this);
    QStyleOptionTab opt;
    for (int i = 0; i < count(); i++)
    {
        initStyleOption(&opt, i);
        painter.drawControl(QStyle::CE_TabBarTabShape, opt);
        painter.save();

        QSize s = opt.rect.size();
        s.transpose();
        QRect r(QPoint(), s);
        r.adjust(0, 8, 0, -8);
        r.moveCenter(opt.rect.center());
        opt.rect = r;

        QPoint c = tabRect(i).center();
        painter.translate(c);
        painter.rotate(90);
        painter.translate(-c);
        painter.drawControl(QStyle::CE_TabBarTabLabel, opt);
        painter.restore();
    }
}
