#ifndef SIGNALCHOOSEWIDGET_H
#define SIGNALCHOOSEWIDGET_H

#include <QWidget>

class SignalChooseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SignalChooseWidget(QStringList &snames, QWidget *parent = nullptr);

signals:

public slots:
};

#endif // SIGNALCHOOSEWIDGET_H
