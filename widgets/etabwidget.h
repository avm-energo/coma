#ifndef ETABWIDGET_H
#define ETABWIDGET_H

#include <QTabWidget>
#include <QTabBar>
#include <QStylePainter>
#include <QStyleOptionTab>
#include <QPainter>
#include <QIcon>
#include "../gen/colors.h"

class ETabBar : public QTabBar
{
public:

    explicit ETabBar(QWidget* parent=0) : QTabBar(parent)
    {
        setIconSize(QSize(18, 18));
        QString tbss = "QTabBar::tab {background-color: " + QString(ACONFGCLR) + ";"
                                     "color: #000000;}"
                       "QTabBar::tab::selected {background-color: " + QString(GRLCOLOR) + ";}";
        this->setStyleSheet(tbss);
    }

protected:
    QSize tabSizeHint(int index) const
    {
        Q_UNUSED(index)
        return QSize(150, 40);
//        QSize s = QTabBar::tabSizeHint(index);
//        s.transpose();
//        return s;
    }
    void paintEvent(QPaintEvent *)
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
            tempIcon.paint(&painter, 0, tabrect.top(), tab.iconSize.width(), tab.iconSize.height(), Qt::AlignTop | Qt::AlignHCenter);
            painter.end();
        } */

        QStylePainter painter(this);
        QStyleOptionTab opt;
        for(int i = 0; i<count(); i++)
        {
            initStyleOption(&opt,i);
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
            painter.drawControl(QStyle::CE_TabBarTabLabel,opt);
            painter.restore();
        }
    }
};

class ETabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit ETabWidget(QWidget *parent = nullptr);

signals:
    void tabClicked(int index);

public slots:

private slots:
    void TabClicked();

};


#endif // ETABWIDGET_H
