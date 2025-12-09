#pragma once

#include "../gen/colors.h"

#include <QIcon>
#include <QPainter>
#include <QProxyStyle>
#include <QStyleOptionTab>
#include <QStylePainter>
#include <QTabBar>
#include <QTabWidget>

class CustomTabStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(
        ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab)
        {
            s.transpose();
        }
        return s;
    }

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel)
        {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
            {
                QStyleOptionTab opt(*tab);
                opt.shape = QTabBar::RoundedNorth;
                QProxyStyle::drawControl(element, &opt, painter, widget);
                return;
            }
        }
        QProxyStyle::drawControl(element, option, painter, widget);
    }
};

class ETabBar : public QTabBar
{
public:
    explicit ETabBar(QWidget *parent = 0) : QTabBar(parent)
    {
        setIconSize(QSize(18, 18));
        QString tbss = "QTabBar::tab {background-color: " + QString(ACONFGCLR)
            + ";"
              "color: #000000;}"
              "QTabBar::tab::selected {background-color: "
            + QString(GRLCOLOR) + ";}";
        setStyle(new CustomTabStyle);
        this->setStyleSheet(tbss);
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
