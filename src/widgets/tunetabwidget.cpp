#include "tunetabwidget.h"

TuneTabWidget::TuneTabWidget()
{
    setObjectName("tunetw");
}

int TuneTabWidget::addTabWidget(QWidget *w, const QString &caption)
{
    MainWidgetStruct str;
    str.widget = w;
    str.caption = caption;
    int widgetindex = m_mainWidgetList.size();
    m_mainWidgetList.append(str);
    return widgetindex;
}

QTabWidget *TuneTabWidget::set()
{
    for (int i = 0; i < m_mainWidgetList.size(); ++i)
        addTab(m_mainWidgetList.at(i).widget, m_mainWidgetList.at(i).caption);
    return this;
}
