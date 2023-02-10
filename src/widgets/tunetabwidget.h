#ifndef TUNETABWIDGET_H
#define TUNETABWIDGET_H

#include <QTabWidget>
#include <QWidget>

class TuneTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    TuneTabWidget();

    int addTabWidget(QWidget *w, const QString &caption);
    QTabWidget *set();

private:
    struct MainWidgetStruct
    {
        QWidget *widget;
        QString caption;
    };
    QList<MainWidgetStruct> m_mainWidgetList;
};

#endif // TUNETABWIDGET_H
