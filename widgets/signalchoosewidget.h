#ifndef SIGNALCHOOSEWIDGET_H
#define SIGNALCHOOSEWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>

class SignalChooseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SignalChooseWidget(QStringList &snames, QWidget *parent = nullptr);

signals:

public slots:
};

class MarkSignalWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MarkSignalWidget(QWidget *parent = nullptr);

signals:
    void Clicked();

protected:
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
}
#endif // SIGNALCHOOSEWIDGET_H
